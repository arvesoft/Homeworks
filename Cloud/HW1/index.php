<form action="" method="get">
    <p>
        Color:
        <select id="colors" name="colors">
            <option value="000" selected>black</option>
            <option value="fff">white</option>
            <option value="f00">red</option>
            <option value="0f0">green</option>
            <option value="00f">blue</option>
        </select>
    </p>
    <p>
        Background color:
        <select id="bgcolors" name="bgcolors">
            <option value="fff" selected>white</option>
            <option value="000" >black</option>
            <option value="f00">red</option>
            <option value="0f0">green</option>
            <option value="00f">blue</option>
        </select>
    </p>
    <input type="text" name="text"/>
    <input type="submit" value="Generate QR Code"/>
</form>

<?php
if(@$_GET){
       $text = $_GET['text'];
       $colors = $_GET['colors'];
       $bgcolors = $_GET['bgcolors'];
          ?>
              <img src="http://api.qrserver.com/v1/create-qr-code/?data=<?=$text?>&size=200x200&color=<?=$colors?>&bgcolor=<?=$bgcolors?>"/>

<?php 
}
?>
<form action="http://api.qrserver.com/v1/read-qr-code/" method="post" enctype="multipart/form-data">
            Choose QR code image: <input name="file" type="file" />
        <input type="submit" value="Read" />
        
</form>

