<?php
  // iot.php
    // Importamos la configuración
    require("config.php");
    // Leemos los valores que nos llegan por GET
    $tempLM35 = mysqli_real_escape_string($con, $_GET['tempLM35']);
    $tempBMP = mysqli_real_escape_string($con, $_GET['tempBMP']);
    $tempDHT = mysqli_real_escape_string($con, $_GET['tempDHT']);
    $altitud = mysqli_real_escape_string($con, $_GET['altitud']);
    $luz = mysqli_real_escape_string($con, $_GET['luz']);
    $humedad = mysqli_real_escape_string($con, $_GET['humedad']);
    $presion = mysqli_real_escape_string($con, $_GET['presion']);
    // Esta es la instrucción para insertar los valores
    $query = "INSERT INTO valores(tempLM35, tempBMP, tempDHT, altitud, luz, humedad, presion) VALUES('".$tempLM35."','".$tempBMP."','".$tempDHT."','".$altitud."','".$luz."','".$humedad."','".$presion."')";
    // Ejecutamos la instrucción
    mysqli_query($con, $query);
    mysqli_close($con);
?>