function openAlert() {
    window.alert("Hello World");
}

function changeH1Color(){

    document.getElementById("headerH1").style.color = "red";

}

function encenderLED(){

    window.location.href = "https://www.geeksforgeeks.org/";


}

function apagarLED(){

    window.location.href = "https://www.inti.gob.ar/";


}

function login(){

    window.location.href = "/login";


}

function navegarAtras(){

    //window.history.back();
    window.location.href = "../";


}
/*
function interactuarConESP32(){

    //window.history.back();
    //alert("hola");

    document.getElementById("GPIO2").style.color = "black";

    var x = document.getElementById("fname").value;
    var y = document.getElementById("lname").value;
    var GPIO1 = document.getElementById("GPIO1").checked;
    var GPIO2 = document.getElementById("GPIO2").checked;
    //var GPIOprueba = document.getElementById("GPIO2").checked;
    //console.log(GPIOprueba);
    
    /*
    if(x !== null){

        alert(x+"papa");
        console.log(x);
    }
    
    var p = document.getElementById("GPIO2").value;
    
    if(p !== null){
    
        
        alert(p);
        console.log(p);

    }
*/
/*
    if(x == "LED_ON"){
        document.getElementById("GPIO2label").style.color = "green";
    }
    if(x == "LED_OFF"){
        document.getElementById("GPIO2label").style.color = "red";
    }
 */
/*
    if(GPIO1 == "true"){
        document.getElementById("GPIO1label").style.color = "green";
    }

    if(GPIO1 == "false"){
        document.getElementById("GPIO1label").style.color = "red";
    }
    if(GPIO2 == "true"){
        document.getElementById("GPIO2label").style.color = "green";
    }
    if(GPIO2 == "false"){
        document.getElementById("GPIO2label").style.color = "red";

    }
    //window.location.href = "/interactuarConESP32";

    //let formData = new FormData([form]);
    //document.getElementsByClassName("circle1").style.color = "red";
  // add one more field
    //formData.append("middle", "Lee");

    // send it out
    let xhr = new XMLHttpRequest();
    //xhr.setRequestHeader("Accept-Encoding", "UTF-8");

    xhr.open("POST", "./ESP32");
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send("fname="+x+"&lname="+y+"&GPIO1="+GPIO1+"&GPIO2="+GPIO2);
    //xhr.send(formData);
*/
/*
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "POST", "./ESP32", false ); // false for synchronous request
    xmlHttp.send("fname=bar&lname=ipsum");
    return xmlHttp.responseText;
*/
    //window.location.href = "/ESP32";

/*

}
*/



function ESP32(){

    window.location.href = "/ESP32";


}

function camara(){

    window.location.href = "/camara";


}

function cambiarColor() {


    
    if(document.getElementById("GPIO1checkbox").checked == true){

        document.getElementById("GPIO1table").style = "background-color: green";
        //alert("cambiarColor");
        //console.log("cambiarColor");

    }else{
        document.getElementById("GPIO1table").style = "background-color: red";
    }
    if(document.getElementById("GPIO2checkbox").checked == true){

        document.getElementById("GPIO2table").style = "background-color: green";

    }else{
        document.getElementById("GPIO2table").style = "background-color: red";
    }
    if(document.getElementById("GPIO3checkbox").checked == true){

        document.getElementById("GPIO3table").style = "background-color: green";

    }else{
        document.getElementById("GPIO3table").style = "background-color: red";
    }
    if(document.getElementById("GPIO4checkbox").checked == true){

        document.getElementById("GPIO4table").style = "background-color: green";

    }else{
        document.getElementById("GPIO4table").style = "background-color: red";
    }

    interactuarConESP32();//para enviar solicitud http con los pines del micro que deben modificar su estado

}


function interactuarConESP32(){

   
    
    var GPIO1 = document.getElementById("GPIO1checkbox").checked;
    var GPIO2 = document.getElementById("GPIO2checkbox").checked;
    var GPIO3 = document.getElementById("GPIO3checkbox").checked;
    var GPIO4 = document.getElementById("GPIO4checkbox").checked;
    //alert("interactuarConESP");
    //console.log("interactuarConESP");
    
    //let formData = new FormData([form]);
    //document.getElementsByClassName("circle1").style.color = "red";
    // add one more field
    //formData.append("middle", "Lee");


    // send it out
    let xhr = new XMLHttpRequest();
    
    xhr.open("POST", "./ESP32");
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send("GPIO1="+GPIO1+"&GPIO2="+GPIO2+"&GPIO3="+GPIO3+"&GPIO4="+GPIO4);
    //xhr.send(formData);

}

window.onload = (event) => {
    console.log("page is fully loaded");

    
    //chart.render();

    
    var dps = [];

    var chart3 = new CanvasJS.Chart("chartContainer", {
        exportEnabled: true,
        title :{
            text: "ADC"
        },
        axisY: {
            includeZero: false
        },
        data: [{
            //type: "spline",
            type: "line",
            markerSize: 0,
            dataPoints: dps 
        }]
    });

    setInterval(consultarEntradas,1000);
    //setInterval(actualizarPagina,1000);
    setInterval(() => rellenarChart(chart3, dps),1000);



    
};

var responseFromServer;

function consultarEntradas(){

    // send it out
    let xhr = new XMLHttpRequest();
    
    xhr.open("GET", "./consultarEntradasESP32");

    xhr.onreadystatechange = () => {
        // In local files, status is 0 upon success in Mozilla Firefox
        if (xhr.readyState === XMLHttpRequest.DONE) {
          const status = xhr.status;
          if (status === 0 || (status >= 200 && status < 400)) {
            // The request has been completed successfully
            responseFromServer = xhr.responseText;
            console.log(xhr.responseText);
            actualizarPagina(xhr.responseText);
          } else {
            // Oh no! There has been an error with the request!
            console.log("No hubo respuesta");
          }
        }
    };



    //xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send();

    


};

function actualizarPagina(respuesta){

    document.getElementById("lecturaInput").innerHTML = "Entradas: " + respuesta;  

}



var xVal = 0;


function rellenarChart(chart, dataPoints){

    
      
    //var xVal = 0;
    //var xVal = Date.now();

    var yVal = 100;
    //var updateInterval = 1000;
    var dataLength = 200; // number of dataPoints visible at any point

    //var updateChart = function (count) {
        //count = count || 1;
        // count is number of times loop runs to generate random dataPoints.
        //for (var j = 0; j < count; j++) {	
    //yVal = yVal + Math.round(5 + Math.random() *(-5-5));
    
    //yVal = promedioTemp;
    yVal = parseInt(responseFromServer);
    console.log("yVal = " + yVal);
    dataPoints.push({
        x: xVal,
        y: yVal
    });
    xVal++;
    //}
    if (dataPoints.length > dataLength) {
        dataPoints.shift();
    }
    chart.render();
      //};
      
      //updateChart(dataLength); 
      //setInterval(function(){ updateChart() }, updateInterval); 



}


var deg = 0;

function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
}

function rotatePhoto() {
    var img = document.getElementById("photo");
    deg += 90;
    if(isOdd(deg/90)){ document.getElementById("container").className = "vert"; }
    else{ document.getElementById("container").className = "hori"; }
    img.style.transform = "rotate(" + deg + "deg)";
}

function isOdd(n) { return Math.abs(n % 2) == 1; }


function captureCont() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/captureCont", true);
    xhr.send();
}

function stopCaptureCont() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/stopCaptureCont", true);
    xhr.send();
}


//var flagFotoContinua = 1;
var intervalID = 0;

function takeContinuousPhoto(){

/*
    if(!intervalID){
        intervalID = setInterval(capturePhoto, 5000);
    }
  */
 
    captureCont();
    
    //window.location.reload();


}

function stopContinuousPhoto(){

    //flagFotoContinua = 0;

    stopCaptureCont();
    /*
    clearInterval(intervalID);
    intervalID = null;
    */
}

function cambiarColoresRGB(){


    window.location.href = "/cambiarColoresRGB";


}

function encenderRuletaRGB(){

    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/encenderRuletaRGB", true);
    xhr.send();

}