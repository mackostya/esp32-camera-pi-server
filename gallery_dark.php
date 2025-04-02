<!-- 
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  Modified by Kostiantyn Lavronenko
  Modified to create dark theme for the gallery
-->

<!DOCTYPE html>
<html>
<head>
  <title>ESP32-CAM Photo Gallery</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    /* Basic Dark Mode Reset */
    html, body {
      margin: 0;
      padding: 0;
      background-color: #121212;
      color: #ffffff;
      font-family: Arial, sans-serif;
    }
    h2 {
      text-align: center;
      padding: 20px 0;
      margin: 0;
      font-weight: 400;
    }
    .flex-container {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      margin: 0 auto;
      max-width: 1200px;
      padding: 20px;
    }
    .flex-container > div {
      background-color: #1f1f1f;
      border: 1px solid #333;
      border-radius: 8px;
      text-align: center;
      margin: 10px;
      padding: 10px;
      width: 350px;
      box-shadow: 0 0 10px rgba(0,0,0,0.5);
      transition: transform 0.2s ease;
    }
    .flex-container > div:hover {
      transform: scale(1.02);
    }
    .flex-container img {
      width: 100%;
      border-radius: 6px;
      margin-top: 10px;
    }
    .delete-link {
      display: inline-block;
      background-color: #ff6f61;
      color: #ffffff;
      padding: 6px 10px;
      border-radius: 4px;
      text-decoration: none;
      margin-bottom: 8px;
      transition: background-color 0.2s ease;
    }
    .delete-link:hover {
      background-color: #ff3f2f;
    }
    a {
      color: #ff6f61;
      text-decoration: none;
    }
    a:hover {
      text-decoration: underline;
    }
    p {
      margin: 0.5em 0;
    }
  </style>
</head>
<body>
<h2>ESP32-CAM Photo Gallery</h2>

<?php
  // Image extensions
  $image_extensions = array("png","jpg","jpeg","gif");

  // Check delete HTTP GET request - remove images
  if(isset($_GET["delete"])){
    $imageFileType = strtolower(pathinfo($_GET["delete"],PATHINFO_EXTENSION));
    if (file_exists($_GET["delete"]) && ($imageFileType == "jpg" ||  $imageFileType == "png" ||  $imageFileType == "jpeg") ) {
      echo "File found and deleted: " .  $_GET["delete"];
      unlink($_GET["delete"]);
    }
    else {
      echo 'File not found - <a href="gallery_dark.php">refresh</a>';
    }
  }
  
  // Target directory
  $dir = 'uploads/';
  if (is_dir($dir)){
    echo '<div class="flex-container">';
    $count = 1;
    $files = scandir($dir);
    rsort($files);
    foreach ($files as $file) {
      if ($file != '.' && $file != '..') { ?>
        <div>
          <p>
            <a class="delete-link" href="gallery_dark.php?delete=<?php echo $dir . $file; ?>">
              Delete file
            </a> 
            - <?php echo $file; ?>
          </p>
          <a href="<?php echo $dir . $file; ?>">
            <img src="<?php echo $dir . $file; ?>" alt="" title=""/>
          </a>
        </div>
      <?php
        $count++;
      }
    }
    echo '</div>';
  }
  if($count==1) { 
    echo "<p style='text-align:center;'>No images found</p>"; 
  }
?>
</body>
</html>