#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>

typedef struct {
    int x, y, z;
    int colorId;
} Vec3i;

Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;

void printMatrix(double Matrix[4][4]){

    for(int i = 0; i < 4; i++){

        std::cout << "[ ";

        for(int j = 0; j < 4; j++){

            std::cout << Matrix[i][j] << " ";
        }
        std::cout << " ]" << std::endl;
    }
}


void putColumn(int col_number, double col_0, double col_1, double col_2, double Matrix[4][4]){

    Matrix[0][col_number] = col_0;
    Matrix[1][col_number] = col_1;
    Matrix[2][col_number] = col_2;
}


void putRow(int row_number, double row_0, double row_1, double row_2, double Matrix[4][4]){


    Matrix[row_number][0] = row_0;
    Matrix[row_number][1] = row_1;
    Matrix[row_number][2] = row_2;
}

void putDiagonal(double d_0, double d_1, double d_2, double Matrix[4][4]){
    Matrix[0][0] = d_0;
    Matrix[1][1] = d_1;
    Matrix[2][2] = d_2;
}


void cameraTransformation(const Camera &cam, double M_cam[4][4]){

    double M_e[4][4];
    double M_c[4][4];
    makeIdentityMatrix(M_e);
    makeIdentityMatrix(M_c);

    putColumn(3, -cam.pos.x, -cam.pos.y, -cam.pos.z, M_e);
    putRow(0, cam.u.x, cam.u.y, cam.u.z, M_c);
    putRow(1, cam.v.x, cam.v.y, cam.v.z, M_c);
    putRow(2, cam.w.x, cam.w.y, cam.w.z, M_c);

    multiplyMatrixWithMatrix(M_cam, M_c, M_e);

}


void perspectiveProjection(const Camera &cam, double M_per[4][4]){

    makeIdentityMatrix(M_per);
    putDiagonal( (2 * cam.n) / double((cam.r - cam.l)), (2 * cam.n) / double((cam.t - cam.b)), -(cam.f + cam.n) / double((cam.f - cam.n)), M_per );
    M_per[0][2] = (cam.r + cam.l) / double((cam.r - cam.l));
    M_per[1][2] = (cam.t + cam.b) / double((cam.t - cam.b));
    M_per[3][2] = -1;
    M_per[2][3] = -( 2 * cam.f * cam.n) / double((cam.f - cam.n));
    M_per[3][3] = 0;
}


void viewportTransformation(int nx, int ny, double M_vp[4][4]){
    makeIdentityMatrix(M_vp);
    putDiagonal(nx/2, ny/2.0, 0.5, M_vp);
    putColumn(3, (nx-1)/2.0, (ny-1)/2.0, 0.5, M_vp);
}


void finalTransformation(double M_final[4][4], double M_vp[4][4], double M_per[4][4], double M_cam[4][4]){
    double M_temp[4][4];
    multiplyMatrixWithMatrix(M_temp, M_vp, M_per);
    multiplyMatrixWithMatrix(M_final, M_temp, M_cam);
}


/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

void matrixEq(double M_1[4][4], double M_2[4][4]){

    for(int i = 0; i < 4; i++){

        for(int j = 0; j < 4; j++){

            M_1[i][j] = M_2[i][j];
        }
    }
}

void translationMatrix(double M_2[4][4], int id){

    Translation translation = translations[id];
    makeIdentityMatrix(M_2);
    putColumn(3, translation.tx, translation.ty, translation.tz, M_2);
}

void scalingMatrix(double M_2[4][4], int id){

    Scaling scaling = scalings[id];
    makeIdentityMatrix(M_2);
    putDiagonal(scaling.sx, scaling.sy, scaling.sz, M_2);
}

void rotatingMatrix(double M_2[4][4], int id){

    double T_do[4][4], T_undo[4][4];
    double M_do[4][4], M_undo[4][4];
    double R_x[4][4], R_y[4][4], R_z[4][4];
    double M_temp1[4][4],M_temp2[4][4];
    Vec3 ou, ov, ow;

    makeIdentityMatrix(T_do);
    makeIdentityMatrix(T_undo);
    makeIdentityMatrix(M_do);
    makeIdentityMatrix(M_undo);
    makeIdentityMatrix(R_x);
    makeIdentityMatrix(R_y);
    makeIdentityMatrix(R_z);

    Rotation rotation = rotations[id];

    putColumn(3, -rotation.ux, -rotation.uy, -rotation.uz, T_do);
    putColumn(3, rotation.ux, rotation.uy, rotation.uz, T_undo);

    ou.x = rotation.ux;
    ou.y = rotation.uy;
    ou.z = rotation.uz;

    if( abs(ou.x) <= abs(ou.y) && abs(ou.x) <= abs(ou.z) ){
        ov.x = 0;
        ov.y = -ou.z;
        ov.z = ou.y;
    }
    else if( abs(ou.y) <= abs(ou.z) && abs(ou.y) <= abs(ou.x) ){
        ov.x = -ou.z;
        ov.y = 0;
        ov.z = ou.x;
    }
    else{
        ov.x = -ou.y;
        ov.y = ou.x;
        ov.z = 0;
    }

    ow = crossProductVec3(ou,ov);
    ov = normalizeVec3(ov);
    ow = normalizeVec3(ow);

    putColumn(0, ou.x, ou.y, ou.z, M_undo);
    putColumn(1, ov.x, ov.y, ov.z, M_undo);
    putColumn(2, ow.x, ow.y, ow.z, M_undo);

    putRow(0, ou.x, ou.y, ou.z, M_do);
    putRow(1, ov.x, ov.y, ov.z, M_do);
    putRow(2, ow.x, ow.y, ow.z, M_do);

    putRow(1, 0, cos( rotation.angle * M_PI / 180), -sin( rotation.angle * M_PI / 180), R_x);
    putRow(2, 0, sin( rotation.angle * M_PI / 180), cos( rotation.angle * M_PI / 180), R_x);

    putRow(0, cos( rotation.angle), 0, sin( rotation.angle), R_y);
    putRow(2, -sin( rotation.angle), 0, cos( rotation.angle), R_y);

    putRow(0, cos( rotation.angle), -sin( rotation.angle), 0, R_z);
    putRow(1, sin( rotation.angle), cos( rotation.angle), 0, R_z);



    multiplyMatrixWithMatrix(M_temp1, T_undo, M_undo);
    multiplyMatrixWithMatrix(M_temp2, M_temp1, R_x);
    multiplyMatrixWithMatrix(M_temp1, M_temp2, M_do);
    multiplyMatrixWithMatrix(M_2, M_temp1, T_do);

}


void calculateTransformationModel(double M_transformation[4][4], const Model &model){

    double M_2[4][4];
    double M_1[4][4];
    double M_temp[4][4];

    makeIdentityMatrix(M_temp);

    for(int i = 0; i < model.numberOfTransformations; i++){

        if(model.transformationTypes[i] == 't'){

            translationMatrix(M_2, model.transformationIDs[i]);

        }

        else if(model.transformationTypes[i] == 's'){

            scalingMatrix(M_2, model.transformationIDs[i]);
        }

        else if(model.transformationTypes[i] == 'r'){

            rotatingMatrix(M_2, model.transformationIDs[i]);
        }

        if(i > 0){

            multiplyMatrixWithMatrix(M_temp, M_2, M_1);
            matrixEq(M_1, M_temp);
        }

        else if(i == 0){

            matrixEq(M_1, M_2);
            matrixEq(M_temp, M_2);

        }
    }

    matrixEq(M_transformation, M_temp);

}
double f_01(double x, double y, double x_0, double y_0, double x_1, double y_1){

    return x * (y_0 - y_1) + y * (x_1 - x_0) + x_0 * y_1 - y_0 * x_1;
}

double f_12(double x, double y, double x_1, double y_1, double x_2, double y_2){

    return x * (y_1 - y_2) + y * (x_2 - x_1) + x_1 * y_2 - y_1 * x_2;
}

double f_20(double x, double y, double x_0, double y_0, double x_2, double y_2){

    return x * (y_2 - y_0) + y * (x_0 - x_2) + x_2 * y_0 - y_2 * x_0;
}

int minF(double x, double y, double z){

    int min;

    if(x <= y && x <= z){
        min = x;
    }
    else if(y <= z && y <= x){
        min = y;
    }
    else{
        min = z;
    }

    return round(min);

}

int maxF(double x, double y, double z){

    int max;

    if(x >= y && x >= z){
        max = x;
    }
    else if(y >= z && y >= x){
        max = y;
    }
    else{
        max = z;
    }

    return round(max);

}

int clamp(int color){

    if(color > 255) return 255;
    else if(color < 0) return 0;
    return color;
}


void rasterizationAlgorithm(Vec3 A, Vec3 B, Vec3 C){

    int y_min, y_max, x_min, x_max;

    x_min = minF(A.x, B.x, C.x);
    x_max = maxF(A.x, B.x, C.x);
    y_min = minF(A.y, B.y, C.y);
    y_max = maxF(A.y, B.y, C.y);

    for(int y = y_min; y <= y_max; y++){

        for(int x = x_min; x <= x_max; x++){

            double alfa = f_12(x, y, (B.x), (B.y), (C.x), (C.y)) / f_12((A.x), (A.y), (B.x), (B.y), (C.x), (C.y));
            if(alfa < 0) continue;

            double beta = f_20(x, y, (A.x), (A.y), (C.x), (C.y)) / f_20((B.x), (B.y), (A.x), (A.y), (C.x), (C.y));
            if(beta < 0) continue;

            double gama = f_01(x, y, (A.x), (A.y), (B.x), (B.y)) / f_01((C.x), (C.y), (A.x), (A.y), (B.x), (B.y));
            if(gama < 0) continue;

            double red = alfa * colors[A.colorId].r + beta * colors[B.colorId].r + gama * colors[C.colorId].r;
            double blue = alfa * colors[A.colorId].b + beta * colors[B.colorId].b + gama * colors[C.colorId].b;
            double green = alfa * colors[A.colorId].g + beta * colors[B.colorId].g + gama * colors[C.colorId].g;
            image[x][y].r = clamp(round(red));
            image[x][y].b = clamp(round(blue));
            image[x][y].g = clamp(round(green));
        }
    }
}


Vec3 vectorSubstraction(Vec3 vec1, Vec3 vec2){


    Vec3 result_vector;

    result_vector.x = vec1.x - vec2.x;
    result_vector.y = vec1.y - vec2.y;
    result_vector.z = vec1.z - vec2.z;

    return result_vector;

}


void mpForLine( Vec3i A, Vec3i B ){

  double m;
  if(A.y == B.y) m = 0.5;

  else if(A.x == B.x){

      if(A.y <= B.y) m = 1.5;
      else m = -1.5;
  }

    if(A.x == B.x){

        if(A.y > B.y){
            int temp1 = B.y;
            B.y = A.y;
            A.y = temp1;
            int color1 = B.colorId;
            B.colorId = A.colorId;
            A.colorId = color1;
        }

        int x = B.x;
        Color color = colors[A.colorId];

        Color d_color;
        d_color.r = double((colors[B.colorId].r - colors[A.colorId].r)) / double((B.y - A.y));
        d_color.g = double((colors[B.colorId].g - colors[A.colorId].g)) / double((B.y - A.y));
        d_color.b = double((colors[B.colorId].b - colors[A.colorId].b)) / double((B.y - A.y));

        for( int y = A.y ; y <= B.y ; y++ ){
            image[x][y].r = clamp(round(color.r));
            image[x][y].g = clamp(round(color.g));
            image[x][y].b = clamp(round(color.b));

            color.r += d_color.r;
            color.g += d_color.g;
            color.b += d_color.b;
        }
        return;
    }

    else if(A.y == B.y){
        int y = B.y;
        Color color = colors[A.colorId];

        Color d_color;
        d_color.r = double((colors[B.colorId].r - colors[A.colorId].r)) / double((B.x - A.x));
        d_color.g = double((colors[B.colorId].g - colors[A.colorId].g)) / double((B.x - A.x));
        d_color.b = double((colors[B.colorId].b - colors[A.colorId].b)) / double((B.x - A.x));
        for( int x = A.x ; x <= B.x ; x++ ){
            image[x][y].r = clamp(round(color.r));
            image[x][y].g = clamp(round(color.g));
            image[x][y].b = clamp(round(color.b));

            color.r += d_color.r;
            color.g += d_color.g;
            color.b += d_color.b;
        }
        return;
    }

    else m = ((B.y) - (A.y)) / double(((B.x) - (A.x)));

    if(m > 0 && m <= 1){

        int y = int(A.y);
        double d = (A.y - B.y) + 0.5 * double((B.x - A.x));

        Color c;
        c.r = colors[A.colorId].r;
        c.b = colors[A.colorId].b;
        c.g = colors[A.colorId].g;

        Color dc;

        dc.r = (colors[B.colorId].r - colors[A.colorId].r) / double((B.x - A.x));
        dc.b = (colors[B.colorId].b - colors[A.colorId].b) / double((B.x - A.x));
        dc.g = (colors[B.colorId].g - colors[A.colorId].g) / double((B.x - A.x));

        for(int x = (A.x); x <= (B.x); x++){

            image[x][y].r = clamp(round(c.r));
            image[x][y].b = clamp(round(c.b));
            image[x][y].g = clamp(round(c.g));


            if(d < 0){

                y++;
                d += (A.y - B.y) + (B.x - A.x);

            }

            else{

                d+= (A.y - B.y);

            }
            c.r += dc.r;
            c.b += dc.b;
            c.g += dc.g;
        }
    }

    else if(m > 1){

        int x = int(A.x);
        double d = 0.5 * double((A.y - B.y)) + (B.x - A.x);

        Color c;
        c.r = colors[A.colorId].r;
        c.b = colors[A.colorId].b;
        c.g = colors[A.colorId].g;

        Color dc;

        dc.r = (colors[B.colorId].r - colors[A.colorId].r) / double((B.y - A.y));
        dc.b = (colors[B.colorId].b - colors[A.colorId].b) / double((B.y - A.y));
        dc.g = (colors[B.colorId].g - colors[A.colorId].g) / double((B.y - A.y));

        for(int y = (A.y); y <= (B.y); y++){

            image[x][y].r = clamp(round(c.r));
            image[x][y].b = clamp(round(c.b));
            image[x][y].g = clamp(round(c.g));

            if(d >= 0){

                x++;
                d += (A.y - B.y) + (B.x - A.x);

            }

            else{

                d += (B.x - A.x);

            }
            c.r += dc.r;
            c.b += dc.b;
            c.g += dc.g;
        }

    }

    else if(m < 0 && m >= -1){


        int y = int(A.y);
        double d = (A.y - B.y) - 0.5 * double((B.x - A.x));

        Color c;
        c.r = colors[A.colorId].r;
        c.b = colors[A.colorId].b;
        c.g = colors[A.colorId].g;

        Color dc;

        dc.r = (colors[B.colorId].r - colors[A.colorId].r) / double((B.x - A.x));
        dc.b = (colors[B.colorId].b - colors[A.colorId].b) / double((B.x - A.x));
        dc.g = (colors[B.colorId].g - colors[A.colorId].g) / double((B.x - A.x));

        for(int x = (A.x); x <= (B.x); x++){

            image[x][y].r = clamp(round(c.r));
            image[x][y].b = clamp(round(c.b));
            image[x][y].g = clamp(round(c.g));

            if(d >= 0){

                y--;
                d += (A.y - B.y) - (B.x - A.x);

            }

            else{

                d+= (A.y - B.y);

            }
            c.r += dc.r;
            c.b += dc.b;
            c.g += dc.g;
        }
    }

    else if(m < -1){

        int x = int(A.x);
        double d = 0.5 * double((A.y - B.y)) - (B.x - A.x);

        Color c;
        c.r = colors[A.colorId].r;
        c.b = colors[A.colorId].b;
        c.g = colors[A.colorId].g;

        Color dc;

        dc.r = (colors[B.colorId].r - colors[A.colorId].r) / double((B.y - A.y));
        dc.b = (colors[B.colorId].b - colors[A.colorId].b) / double((B.y - A.y));
        dc.g = (colors[B.colorId].g - colors[A.colorId].g) / double((B.y - A.y));

        for(int y = (A.y); y >= (B.y); y--){

            image[x][y].r = clamp(round(c.r));
            image[x][y].b = clamp(round(c.b));
            image[x][y].g = clamp(round(c.g));

            if(d < 0){

                x++;
                d += (A.y - B.y) - (B.x - A.x);

            }

            else{

                d += -(B.x - A.x);

            }
            c.r -= dc.r;
            c.b -= dc.b;
            c.g -= dc.g;
        }

    }


}

void midpointAlgorithm( Vec3i A, Vec3i B, Vec3i C ){

    if( A.x <= B.x ){
        mpForLine( A, B );
    }
    else{
        mpForLine( B, A );
    }

    if( A.x <= C.x ){
        mpForLine( A, C );
    }
    else{
        mpForLine( C, A );
    }

    if( B.x <= C.x ){
        mpForLine( B, C );
    }
    else{
        mpForLine( C, B );
    }
}

void triangleRasterization(double M_transform[4][4], Triangle triangle, double M_vp[4][4], const Vec3 & eye_vec, int type){

    Vec3 A = vertices[triangle.vertexIds[0]];
    Vec3 B = vertices[triangle.vertexIds[1]];
    Vec3 C = vertices[triangle.vertexIds[2]];

    double A_v4[4], B_v4[4], C_v4[4];
    double A1_v4[4], B1_v4[4], C1_v4[4];

    A_v4[0] = A.x;
    A_v4[1] = A.y;
    A_v4[2] = A.z;
    A_v4[3] = 1;

    B_v4[0] = B.x;
    B_v4[1] = B.y;
    B_v4[2] = B.z;
    B_v4[3] = 1;

    C_v4[0] = C.x;
    C_v4[1] = C.y;
    C_v4[2] = C.z;
    C_v4[3] = 1;





    multiplyMatrixWithVec4d(A1_v4, M_transform, A_v4);
    multiplyMatrixWithVec4d(B1_v4, M_transform, B_v4);
    multiplyMatrixWithVec4d(C1_v4, M_transform, C_v4);

    A.x = (A1_v4[0]);
    A.y = (A1_v4[1]);
    A.z = A1_v4[2];

    B.x = (B1_v4[0]);
    B.y = (B1_v4[1]);
    B.z = B1_v4[2];

    C.x = (C1_v4[0]);
    C.y = (C1_v4[1]);
    C.z = C1_v4[2];

    A1_v4[0] = A1_v4[0] / A1_v4[3];
    A1_v4[1] = A1_v4[1] / A1_v4[3];
    A1_v4[2] = A1_v4[2] / A1_v4[3];
    A1_v4[3] = A1_v4[3] / A1_v4[3];

    B1_v4[0] = B1_v4[0] / B1_v4[3];
    B1_v4[1] = B1_v4[1] / B1_v4[3];
    B1_v4[2] = B1_v4[2] / B1_v4[3];
    B1_v4[3] = B1_v4[3] / B1_v4[3];

    C1_v4[0] = C1_v4[0] / C1_v4[3];
    C1_v4[1] = C1_v4[1] / C1_v4[3];
    C1_v4[2] = C1_v4[2] / C1_v4[3];
    C1_v4[3] = C1_v4[3] / C1_v4[3];





    Vec3 normal_vec = normalizeVec3(crossProductVec3(vectorSubstraction(B, C), vectorSubstraction(A , C)));

    Vec3 new_vec;
    new_vec.x = 0;
    new_vec.y = 0;
    new_vec.z = 0;

    Vec3 fornorm;
    fornorm.x = (A.x + B.x + C.x) / 3;
    fornorm.y = (A.y + B.y + C.y) / 3;
    fornorm.z = (A.z + B.z + C.z) / 3;

    int cntrl = dotProductVec3(normal_vec, vectorSubstraction(fornorm, new_vec));



    multiplyMatrixWithVec4d(A_v4, M_vp ,A1_v4);
    multiplyMatrixWithVec4d(B_v4, M_vp ,B1_v4);
    multiplyMatrixWithVec4d(C_v4, M_vp ,C1_v4);

    A.x = (A_v4[0]);
    A.y = (A_v4[1]);
    A.z = A_v4[2];

    B.x = (B_v4[0]);
    B.y = (B_v4[1]);
    B.z = B_v4[2];

    C.x = (C_v4[0]);
    C.y = (C_v4[1]);
    C.z = C_v4[2];


    Vec3i A_i, B_i, C_i;

    if((backfaceCullingSetting != 0 &&  cntrl < 0 ) || backfaceCullingSetting == 0){

        if( type == 1 ) rasterizationAlgorithm(A, B, C);
        else if( type == 0 ){

            A_i.x = int(A.x);
            A_i.y = int(A.y);
            A_i.z = int(A.z);
            A_i.colorId = round(A.colorId);

            B_i.x = int(B.x);
            B_i.y = int(B.y);
            B_i.z = int(B.z);
            B_i.colorId = round(B.colorId);

            C_i.x = int(C.x);
            C_i.y = int(C.y);
            C_i.z = int(C.z);
            C_i.colorId = round(C.colorId);

            midpointAlgorithm(A_i, B_i, C_i);
         }
    }



}

void transformModels(const Camera &cam, double M_percam[4][4], double M_vp[4][4]){


    double M_temp[4][4];
    for(int i = 0; i < numberOfModels; i++){

        double M_transformation[4][4];
        makeIdentityMatrix(M_transformation);
        calculateTransformationModel(M_transformation, models[i]);
        multiplyMatrixWithMatrix(M_temp, M_percam, M_transformation);

        for(int j = 0; j < models[i].numberOfTriangles; j++){

            triangleRasterization(M_temp, models[i].triangles[j], M_vp, cam.pos, models[i].type);

        }
    }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE


    double M_cam[4][4];
    double M_per[4][4];
    double M_vp[4][4];
    double M_percam[4][4];

    cameraTransformation(cam, M_cam);
    perspectiveProjection(cam, M_per);
    viewportTransformation(cam.sizeX, cam.sizeY, M_vp);
    multiplyMatrixWithMatrix(M_percam, M_per, M_cam);

    transformModels(cam, M_percam, M_vp);



}


int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {
    //for (i = 0; i < 1; i++) {

        // allocate memory for image
        if (image) {
			for (j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 1);
    }
    return 0;

}
