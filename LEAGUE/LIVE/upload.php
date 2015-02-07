<?php
  include "./upload_conf.php";
  if ($password == $_POST["password"]) {
    $filename = $_POST["filename"];
    $upload = $_FILES["upload"];
    if ($upload["error"] == UPLOAD_ERR_OK) {
      if (file_exists($filename)) {
        unlink($filename);
      }
      $name = $upload["name"];
      move_uploaded_file($upload["tmp_name"], $filename);
    }
  }
?>