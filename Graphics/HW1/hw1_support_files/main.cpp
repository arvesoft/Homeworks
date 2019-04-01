#include <iostream>
#include <limits>
#include <cmath>
#include <thread>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

void vectorWriter(parser::Vec3f);
parser::Vec3i rayShooter(parser::Ray ray, parser::Vec3f ,const parser::Scene &scene);
parser::Vec3f vectorCrossProduct(parser::Vec3f, parser::Vec3f);
float vectorDotProduct(parser::Vec3f, parser::Vec3f);
parser::Vec3f vectorSubstraction(parser::Vec3f, parser::Vec3f);
parser::Vec3f vectorScalarMult(parser::Vec3f, float);
parser::Vec3f vectorAddition(parser::Vec3f, parser::Vec3f);
float determinantor(float, float, float,
                    float, float, float,
                    float, float, float);
parser::Vec3f vectorScalarMult(parser::Vec3f, float);
parser::Vec2f sphereCheck(parser::Ray, parser::Vec3f, const parser::Scene &);


float determinantor(float a, float b, float c,
                    float d, float e, float f,
                    float g, float h, float i){

    return a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);

}

parser::Vec3f vectorScalarMult(parser::Vec3f vec, float num){

    vec.x *= num;
    vec.y *= num;
    vec.z *= num;
    return vec;

}

parser::Vec3f vectorScalarAdd(parser::Vec3f vec, float num){

    vec.x += num;
    vec.y += num;
    vec.z += num;
    return vec;

}

parser::Vec2f sphereCheck(parser::Ray ray, parser::Vec3f cam_position, const parser::Scene &scene){

    int sphere_num = scene.spheres.size();

    float A =  vectorDotProduct(ray.ray_dir, ray.ray_dir);
    float min_distance = std::numeric_limits<float>::max();
    float min_sphere_id = -1;

    for(int sphere_id = 0; sphere_id < sphere_num; sphere_id++){

        parser::Sphere sphere = scene.spheres[sphere_id];
        parser::Vec3f center_vec = scene.vertex_data[sphere.center_vertex_id - 1];
        parser::Vec3f e_c_vector = vectorSubstraction(cam_position , center_vec);
        float R2 = sphere.radius * sphere.radius;
        float B = 2 * vectorDotProduct(ray.ray_dir, e_c_vector);
        float C = vectorDotProduct(e_c_vector, e_c_vector) - R2;
        float discriminant = B * B - 4 * A * C;

        if(discriminant >= 0){

            float t1, t2, t_min;

            t1 = (-B + sqrt(discriminant)) / 2 * A;

            if(discriminant != 0){
                t2 = (-B - sqrt(discriminant)) / 2 * A;
                if(t1 > t2) t_min = t2;
                else t_min = t1;
            }

            else{
                t_min = t1;
            }

            if(t_min < min_distance && t_min > 0){
                min_distance = t_min;
                min_sphere_id = sphere_id;
            }

        }

    }

    parser::Vec2f return_vec;
    return_vec.x = min_sphere_id;
    return_vec.y = min_distance;
    return return_vec;

}


parser::Vec3f meshCheck(parser::Ray ray, parser::Vec3f cam_position,const parser::Scene &scene){

    int mesh_size = scene.meshes.size();
    parser::Vec3f return_vec;
    float mesh_id = -1;
    float face_id = -1;
    float min_distance = std::numeric_limits<float>::max();

    for(int j = 0; j < mesh_size; j++){

        int face_size = scene.meshes[j].faces.size();
        for(int i = 0; i < face_size; i++){

            parser::Face face = scene.meshes[j].faces[i];

            parser::Vec3f triA, triB, triC;

            triA = scene.vertex_data[face.v0_id - 1];
            triB = scene.vertex_data[face.v1_id - 1];
            triC = scene.vertex_data[face.v2_id - 1];

            float detA = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
                triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
                ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z);

            float beta = determinantor(triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z,
                            triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
                            ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z) / detA;
                            //std::cout << "DETA = " << detA << std::endl;
                            //std::cout << "BETA = " << beta << std::endl;
            if(beta < 0) continue;

            float gama = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
                            triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z,
                            ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z) / detA;
                            //std::cout << "GAMA = " << gama << std::endl;
            if(gama < 0) continue;

            float teta = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
                            triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
                            triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z) / detA;
            if(beta + gama > 1) continue;

            if(teta < min_distance && teta > 0){

                min_distance = teta;
                mesh_id = j;
                face_id = i;
            }

        }
    }

    return_vec.x = mesh_id;
    return_vec.y = face_id;
    return_vec.z = min_distance;

    return return_vec;

}

parser::Vec2f triangleCheck(parser::Ray ray, parser::Vec3f cam_position,const parser::Scene &scene){

    int triangle_size = scene.triangles.size();
    parser::Vec2f return_vec;
    float triangle_id = -1;
    float min_distance = std::numeric_limits<float>::max();

    for(int i = 0; i < triangle_size; i++){

        parser::Triangle triangle = scene.triangles[i];

        parser::Vec3f triA, triB, triC;

        triA = scene.vertex_data[triangle.indices.v0_id - 1];
        triB = scene.vertex_data[triangle.indices.v1_id - 1];
        triC = scene.vertex_data[triangle.indices.v2_id - 1];

        float detA = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
            triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
            ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z);

        float beta = determinantor(triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z,
                        triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
                        ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z) / detA;

        if(beta < 0) continue;

        float gama = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
                        triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z,
                        ray.ray_dir.x, ray.ray_dir.y, ray.ray_dir.z) / detA;
        if(gama < 0) continue;

        float teta = determinantor(triA.x - triB.x, triA.y - triB.y, triA.z - triB.z,
                        triA.x - triC.x, triA.y - triC.y, triA.z - triC.z,
                        triA.x - cam_position.x, triA.y - cam_position.y, triA.z - cam_position.z) / detA;

        if(beta + gama > 1) continue;


        if(teta < min_distance && teta > 0){

            min_distance = teta;
            triangle_id = i;
        }

    }

    return_vec.x = triangle_id;
    return_vec.y = min_distance;

    return return_vec;

}

parser::Vec3f uniter(parser::Vec3f vec){

    return vectorScalarMult(vec, 1 / sqrt(vectorDotProduct(vec,vec)));
}

parser::Vec3f RGBmult(parser::Vec3f vec1, parser::Vec3f vec2){

    parser::Vec3f return_vec;

    return_vec.x = vec1.x * vec2.x;
    return_vec.y = vec1.y * vec2.y;
    return_vec.z = vec1.z * vec2.z;
    return return_vec;
}

parser::Vec3i shader(parser::Vec3f ambient_co,parser::Vec3f diffuse_co, parser::Vec3f specular_co, parser::Vec3f reflection_co, float phong_exponent, parser::Vec3f normal_vec,
                            parser::Vec3f intersect_point, parser::Ray ray,const parser::Scene &scene){
    parser::Vec3f E_diffuse, E_total, E_specular;
    parser::Vec3i E_final;
    float zero = 0;
    ray.ray_dir = vectorScalarMult(ray.ray_dir, -1);
    int light_size = scene.point_lights.size();
    for( int i = 0 ; i < light_size ; i++ ){
        parser::Ray light_ray;
        parser::Vec3f light_pos = scene.point_lights[i].position;
        parser::Vec3f light_int = scene.point_lights[i].intensity;
        parser::Vec3f light_vec = vectorSubstraction(light_pos, intersect_point);
        float light_dis = sqrt( vectorDotProduct(light_vec, light_vec));
        light_vec = uniter(light_vec);


        light_ray.ray_dir = light_vec;
        parser::Vec3f shadow_intersection = vectorAddition(intersect_point,
            vectorScalarMult(light_ray.ray_dir, scene.shadow_ray_epsilon));

        //std::cout <<  "org" << std::endl;
        //vectorWriter(intersect_point);
        //std::cout <<  "new" << std::endl;
        //vectorWriter(shadow_intersection);

        parser::Vec3f sub_vec = vectorSubstraction(light_pos,shadow_intersection);
        float tmax = sqrt(vectorDotProduct(sub_vec, sub_vec));



        parser::Vec2f sphere_res = sphereCheck(light_ray, shadow_intersection, scene);
        if(sphere_res.x > -0.1 && sphere_res.y <= tmax) continue;
        parser::Vec2f triangle_res = triangleCheck(light_ray, shadow_intersection, scene);
        if(triangle_res.x > -0.1 && triangle_res.y <= tmax) continue;
        parser::Vec3f mesh_res = meshCheck(light_ray, shadow_intersection, scene);
        if(mesh_res.x > -0.1 && mesh_res.z<= tmax) continue;



        /*std::cout << "Normal" << std::endl;
        vectorWriter(normal_vec);
        std::cout << "Light"<< std::endl;
        vectorWriter(light_vec);*/


        // precompute
        float pre_val_dif = std::max(zero, vectorDotProduct(light_vec, normal_vec)) / (light_dis * light_dis);

        E_diffuse.x = diffuse_co.x * pre_val_dif * light_int.x;
        E_diffuse.y = diffuse_co.y * pre_val_dif * light_int.y;
        E_diffuse.z = diffuse_co.z * pre_val_dif * light_int.z;

        parser::Vec3f temp_vec = vectorAddition(light_vec, ray.ray_dir);
        parser::Vec3f half_vec = uniter(temp_vec);

        //std::cout << "half_vec";
        //vectorWriter(half_vec);
        //std::cout << "normal_vec";
        //vectorWriter(normal_vec);

        // precompute
        float pre_val_spe = pow(std::max(zero, vectorDotProduct(half_vec, normal_vec)), phong_exponent) / (light_dis * light_dis);

        E_specular.x = specular_co.x * pre_val_spe * light_int.x;
        E_specular.y = specular_co.y * pre_val_spe * light_int.y;
        E_specular.z = specular_co.z * pre_val_spe * light_int.z;


        E_total.x += E_diffuse.x + E_specular.x;
        E_total.y += E_diffuse.y + E_specular.y;
        E_total.z += E_diffuse.z + E_specular.z;
    }

    parser::Vec3f L_ambient = RGBmult(ambient_co, scene.ambient_light);

    E_final.x = round(E_total.x + L_ambient.x);
    if(E_final.x > 255) E_final.x = 255;
    if(E_final.x < 0) E_final.x = 0;
    E_final.y = round(E_total.y + L_ambient.y);
    if(E_final.y > 255) E_final.y = 255;
    if(E_final.y < 0) E_final.y = 0;
    E_final.z = round(E_total.z + L_ambient.z);
    if(E_final.z > 255) E_final.z = 255;
    if(E_final.z < 0) E_final.z = 0;

    if(reflection_co.x == 0 && reflection_co.y == 0 && reflection_co.z == 0)
        return E_final;




    parser::Vec3f wr_vec = vectorAddition(vectorScalarMult(ray.ray_dir, -1), vectorScalarMult(normal_vec, 2 * vectorDotProduct(normal_vec, ray.ray_dir)));

    wr_vec = uniter(wr_vec);
    parser::Vec3f temmp = wr_vec;

    parser::Ray wr_ray;
    wr_ray.ray_dir = wr_vec;
    wr_ray.total_reflection = ray.total_reflection;


    parser::Vec3f reflection_intersection = vectorAddition(intersect_point,
        vectorScalarMult(wr_vec, scene.shadow_ray_epsilon));


    parser::Vec2f sphere_res = sphereCheck(wr_ray, reflection_intersection, scene);
    parser::Vec2f triangle_res = triangleCheck(wr_ray, reflection_intersection, scene);
    parser::Vec3f mesh_res = meshCheck(wr_ray, reflection_intersection, scene);
    if(mesh_res.x < -0.1 && sphere_res.x < -0.1 && triangle_res.x < -0.1) return E_final;


    float min_distance;
    int surface_type = -1;

    parser::Vec3i L_diffuse;
    parser::Material material;
    if(sphere_res.y < triangle_res.y){
        min_distance = sphere_res.y;
        surface_type = 0;
    }
    else{
        surface_type = 1;
        min_distance = triangle_res.y;
    }

    if(mesh_res.z < min_distance){
        min_distance = mesh_res.z;
        surface_type = 2;
    }

    parser::Vec3f ambient_co_next;
    parser::Vec3f reflection_co_next;

    parser::Vec3f new_intersection = vectorAddition(reflection_intersection, vectorScalarMult(temmp, min_distance));
    parser::Vec3f temp = vectorSubstraction(new_intersection, reflection_intersection);
    float R_w = sqrt( vectorDotProduct(temp, temp));

    if(surface_type == 0){
        parser::Sphere sphere = scene.spheres[sphere_res.x];
        material = scene.materials[sphere.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co_next = material.mirror;
        ambient_co_next = material.ambient;

        normal_vec = vectorSubstraction(new_intersection, scene.vertex_data[sphere.center_vertex_id - 1]);
        normal_vec = vectorScalarMult(normal_vec, 1 / sphere.radius);


    }
    else if(surface_type == 1){ // diffuse_co, normal, intersect, ray, scene
        parser::Triangle triangle = scene.triangles[triangle_res.x];
        normal_vec = triangle.indices.normal_vector;
        material = scene.materials[triangle.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co_next = material.mirror;
        ambient_co_next = material.ambient;
    }
    else if(surface_type == 2){
        parser::Mesh mesh = scene.meshes[mesh_res.x];
        parser::Face face = mesh.faces[mesh_res.y];
        normal_vec = face.normal_vector;
        material = scene.materials[mesh.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co_next = material.mirror;
        ambient_co_next = material.ambient;
    }



    // float R_w = sqrt( vectorDotProduct(vectorScalarMult(temmp, min_distance), vectorScalarMult(temmp, min_distance)));


    parser::Vec3f E_reflection;
    if(wr_ray.total_reflection < scene.max_recursion_depth){
        wr_ray.total_reflection = ray.total_reflection + 1;
        parser::Vec3i shader_res = shader(ambient_co_next, diffuse_co, specular_co, reflection_co_next,
            phong_exponent, normal_vec, new_intersection, wr_ray, scene);
            //std::cout << shader_res.x << " | " << shader_res.y << " | " << shader_res.z << std::endl;
        E_reflection.x = reflection_co.x * shader_res.x;

        E_reflection.y = reflection_co.y * shader_res.y;

        E_reflection.z = reflection_co.z * shader_res.z;

    }



    E_final.x += round(E_reflection.x);
    E_final.y += round(E_reflection.y);
    E_final.z += round(E_reflection.z);



    if(E_final.x > 255) E_final.x = 255;
    if(E_final.x < 0) E_final.x = 0;
    if(E_final.y > 255) E_final.y = 255;
    if(E_final.y < 0) E_final.y = 0;
    if(E_final.z > 255) E_final.z = 255;
    if(E_final.z < 0) E_final.z = 0;


    return E_final;
}

parser::Vec3i rayShooter(parser::Ray ray, parser::Vec3f cam_position,const parser::Scene &scene){

    parser::Vec3i return_vec;
    parser::Vec2f sphere_res = sphereCheck(ray, cam_position,scene);
    parser::Vec2f triangle_res = triangleCheck(ray, cam_position, scene);
    parser::Vec3f mesh_res = meshCheck(ray, cam_position, scene);
    float min_distance ,phong_exponent;
    // Surface type 0 = sphere, 1 = triangle, 2 = mesh
    int surface_type = -1;


    if(sphere_res.x < -0.1 && triangle_res.x < -0.1 && mesh_res.x < -0.1 ){
        return scene.background_color;
    }

    else{
        if(sphere_res.y < triangle_res.y){
            min_distance = sphere_res.y;
            surface_type = 0;
        }
        else{
            surface_type = 1;
            min_distance = triangle_res.y;
        }

        if(mesh_res.z < min_distance){
            min_distance = mesh_res.z;
            surface_type = 2;
        }
    }

    parser::Vec3f diffuse_co, normal_vec, intersect_point, specular_co;
    float light_distance;

    intersect_point.x = cam_position.x + ray.ray_dir.x * min_distance;
    intersect_point.y = cam_position.y + ray.ray_dir.y * min_distance;
    intersect_point.z = cam_position.z + ray.ray_dir.z * min_distance;

    parser::Vec3i L_diffuse;
    parser::Vec3f L_ambient;
    parser::Material material;
    parser::Vec3f reflection_co;
    if(surface_type == 0){

        parser::Sphere sphere = scene.spheres[sphere_res.x];
        material = scene.materials[sphere.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co = material.mirror;

        normal_vec = vectorSubstraction(intersect_point, scene.vertex_data[sphere.center_vertex_id - 1]);
        normal_vec = vectorScalarMult(normal_vec, 1 / sphere.radius);


    }
    else if(surface_type == 1){ // diffuse_co, normal, intersect, ray, scene
        parser::Triangle triangle = scene.triangles[triangle_res.x];
        normal_vec = triangle.indices.normal_vector;
        material = scene.materials[triangle.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co = material.mirror;
    }
    else if(surface_type == 2){
        parser::Mesh mesh = scene.meshes[mesh_res.x];
        parser::Face face = mesh.faces[mesh_res.y];
        normal_vec = face.normal_vector;
        material = scene.materials[mesh.material_id - 1];
        diffuse_co = material.diffuse;
        specular_co = material.specular;
        phong_exponent = material.phong_exponent;
        reflection_co = material.mirror;
    }

    //L_ambient = RGBmult(material.ambient, scene.ambient_light);
    L_diffuse = shader(material.ambient ,diffuse_co, specular_co, reflection_co, phong_exponent, normal_vec, intersect_point, ray, scene);
    //L_specular = speculerShader(specular_co, normal_vec, intersect_point, ray, scene);


    return_vec.x = L_diffuse.x + round(L_ambient.x);
    if(return_vec.x > 255) return_vec.x = 255;

    return_vec.y = L_diffuse.y + round(L_ambient.y);
    if(return_vec.y > 255) return_vec.y = 255;

    return_vec.z = L_diffuse.z + round(L_ambient.z);
    if(return_vec.z > 255) return_vec.z = 255;

    return return_vec;
}


parser::Vec3f vectorSubstraction(parser::Vec3f vec1, parser::Vec3f vec2){


    parser::Vec3f result_vector;

    result_vector.x = vec1.x - vec2.x;
    result_vector.y = vec1.y - vec2.y;
    result_vector.z = vec1.z - vec2.z;

    return result_vector;

}

parser::Vec3f vectorAddition(parser::Vec3f vec1, parser::Vec3f vec2){


    parser::Vec3f result_vector;

    result_vector.x = vec1.x + vec2.x;
    result_vector.y = vec1.y + vec2.y;
    result_vector.z = vec1.z + vec2.z;

    return result_vector;

}


parser::Vec3f vectorCrossProduct(parser::Vec3f vec1, parser::Vec3f vec2){

    parser::Vec3f result_vector;
    float u1, u2, u3, v1, v2, v3;

    u1 = vec1.x;
    u2 = vec1.y;
    u3 = vec1.z;
    v1 = vec2.x;
    v2 = vec2.y;
    v3 = vec2.z;

    result_vector.x = u2 * v3 - v2 * u3;
    result_vector.y = v1 * u3 - u1 * v3;
    result_vector.z = u1 * v2 - v1 * u2;


    return result_vector;

}

float vectorDotProduct(parser::Vec3f vec1, parser::Vec3f vec2){


    return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}


void vectorWriter(parser::Vec3f vec){

    std::cout << "X = " << vec.x << " | Y = " << vec.y << " | Z = " << vec.z << std::endl;
}

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;


    scene.loadFromXml(argv[1]);



    //Triangle normallerini hesaplama
    std::cout << "Triangle size  = " << scene.triangles.size() << std::endl;
    for(int i = 0; i < scene.triangles.size(); i++){

        //First vector is C calculate B-C and A-C

        int C = scene.triangles[i].indices.v0_id;
        int B = scene.triangles[i].indices.v1_id;
        int A = scene.triangles[i].indices.v2_id;

        std::cout << C << " " << B << " " << A << std::endl;

        scene.triangles[i].indices.normal_vector = uniter(vectorCrossProduct(
                vectorSubstraction(scene.vertex_data[B - 1] , scene.vertex_data[C - 1]),
                vectorSubstraction(scene.vertex_data[A - 1],scene.vertex_data[C - 1])));

        //std::cout << "Normal Vector : ";
        //vectorWriter(scene.triangles[i].indices.normal_vector);

    }

    //Mesh icindeki face normallerini hesaplama
    std::cout << "Mesh size  = " << scene.meshes.size() << std::endl;
    for(int i = 0; i < scene.meshes.size(); i++){


        parser::Mesh mesh = scene.meshes[i];
        int face_size = mesh.faces.size();

        std::cout << "Mesh face size  = " << face_size << std::endl;

        for(int j = 0; j < face_size; j++){


            int C = mesh.faces[j].v0_id;
            int B = mesh.faces[j].v1_id;
            int A = mesh.faces[j].v2_id;

            scene.meshes[i].faces[j].normal_vector = uniter(vectorCrossProduct(
                    vectorSubstraction(scene.vertex_data[B - 1] , scene.vertex_data[C - 1]),
                    vectorSubstraction(scene.vertex_data[A - 1],scene.vertex_data[C - 1])));



        }
    }



    int cores = std::thread::hardware_concurrency();
    std::cout << "cores = " << cores << std::endl;
    std::cout << "Camera size = " << scene.cameras.size() << std::endl;
    for(int cam_id = 0 ; cam_id < scene.cameras.size(); cam_id++){

        std::cout << std::endl << "Camera id = " << cam_id << std::endl;
        parser::Camera camera = scene.cameras[cam_id];
        parser::Vec3f cam_position = scene.cameras[cam_id].position;
        std::cout << "Cam position" << std::endl;
        vectorWriter(cam_position);

        float left, right, bottom, top, near_distance;
        int ny, nx;

        left = camera.near_plane.x;
        right = camera.near_plane.y;
        bottom = camera.near_plane.z;
        top = camera.near_plane.w;
        nx = camera.image_width;
        ny = camera.image_height;
        near_distance = camera.near_distance;
        const char* image_name = camera.image_name.c_str();
        unsigned char image[ny * nx * 3];

        parser::Vec3f orientation_vec, cross_vec, new_vec, up_vec, dw_vector, w_vector, u_vector, v_vector;

        w_vector = uniter(vectorScalarMult(camera.gaze, -1));
        u_vector = uniter(vectorCrossProduct(camera.up, w_vector));
        v_vector = vectorCrossProduct(w_vector, u_vector);
        dw_vector = vectorScalarMult(vectorScalarMult(w_vector, -1), near_distance);


        int pixel_3 = 0;
        for (int y = 0; y < ny; ++y){

            for (int x = 0; x < nx; ++x){

                float u = left + (right - left) * (x + 0.5) / nx;
                float v = top - (top - bottom) * (y + 0.5) / ny;
                new_vec = vectorScalarMult(u_vector, u);
                up_vec = vectorScalarMult(v_vector, v);
                orientation_vec = vectorAddition(dw_vector, up_vec);
                orientation_vec = vectorAddition(orientation_vec, new_vec);
                //orientation_vec = vectorAddition(cam_position, orientation_vec);
                parser::Ray ray;
                ray.ray_dir = uniter(orientation_vec);
                parser::Vec3i col_vec = rayShooter(ray, cam_position, scene);
                image[pixel_3++] = col_vec.x;
                image[pixel_3++] = col_vec.y;
                image[pixel_3++] = col_vec.z;

            }
        }
        write_ppm(image_name, image, nx, ny);
    }




}
