/**
 * Add gobals here
 */
var seconds 	= null;
var otaTimerVar =  null;
var wifiConnectInterval = null;

/**
 * Initialize functions here.
 */
$(document).ready(function(){
	//getUpdateStatus();
	//startDHTSensorInterval();
	$("#connect_wifi").on("click", function(){
		checkCredentials();
	}); 
});   

/**
 * Gets file name and size for display on the web page.
 */        
function getFileInfo() 
{
    var x = document.getElementById("selected_file");
    var file = x.files[0];

    document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() 
{
    // Form Data
    var formData = new FormData();
    var fileSelect = document.getElementById("selected_file");
    
    if (fileSelect.files && fileSelect.files.length == 1) 
	{
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

        // Http Request
        var request = new XMLHttpRequest();

        request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
    } 
	else 
	{
        window.alert('Select A File First')
    }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) 
{
    if (oEvent.lengthComputable) 
	{
        getUpdateStatus();
    } 
	else 
	{
        window.alert('total size is unknown')
    }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) 
	{		
        var response = JSON.parse(xhr.responseText);
						
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1) 
		{
    		// Set the countdown timer time
            seconds = 10;
            // Start the countdown timer
            otaRebootTimer();
        } 
        else if (response.ota_update_status == -1)
		{
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() 
{	
    document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;

    if (--seconds == 0) 
	{
        clearTimeout(otaTimerVar);
        window.location.reload();
    } 
	else 
	{
        otaTimerVar = setTimeout(otaRebootTimer, 1000);
    }
}

/**
 * Gets DHT22 sensor temperature and humidity values for display on the web page.
 */
function getDHTSensorValues()
{
	$.getJSON('/dhtSensor.json', function(data) {
		$("#temperature_reading").text(data["temp"]);
		$("#humidity_reading").text(data["humidity"]);
	});
}

/**
 * Sets the interval for getting the updated DHT22 sensor values.
 */
function startDHTSensorInterval()
{
	setInterval(getDHTSensorValues, 5000);    
}

/**
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval()
{
	if (wifiConnectInterval != null)
	{
		clearInterval(wifiConnectInterval);
		wifiConnectInterval = null;
	}
}

/**
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus()
{
	var xhr = new XMLHttpRequest();
	var requestURL = "/wifiConnectStatus";
	xhr.open('POST', requestURL, false);
	xhr.send('wifi_connect_status');
	
	if (xhr.readyState == 4 && xhr.status == 200)
	{
		var response = JSON.parse(xhr.responseText);
		
		document.getElementById("wifi_connect_status").innerHTML = "Connecting...";
		
		if (response.wifi_connect_status == 2)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatibility</h4>";
			stopWifiConnectStatusInterval();
		}
		else if (response.wifi_connect_status == 3)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
			stopWifiConnectStatusInterval();
		}
	}
}

/**
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval()
{
	wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
}

/**
 * Connect WiFi function called using the SSID and password entered into the text fields.
 */
function connectWifi()
{
	// Get the SSID and password
	/*selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	$.ajax({
		url: '/wifiConnect.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		headers: {'my-connect-ssid': selectedSSID, 'my-connect-pwd': pwd},
		data: {'timestamp': Date.now()}
	});
	*/
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	// Create an object to hold the data to be sent in the request body
	var requestData = {
	  'selectedSSID': selectedSSID,
	  'pwd': pwd,
	  'timestamp': Date.now()
	};
	
	// Serialize the data object to JSON
	var requestDataJSON = JSON.stringify(requestData);
	
	$.ajax({
	  url: '/wifiConnect.json',
	  dataType: 'json',
	  method: 'POST',
	  cache: false,
	  data: requestDataJSON, // Send the JSON data in the request body
	  contentType: 'application/json', // Set the content type to JSON
	  success: function(response) {
		// Handle the success response from the server
		console.log(response);
	  },
	  error: function(xhr, status, error) {
		// Handle errors
		console.error(xhr.responseText);
	  }
	});


	//startWifiConnectStatusInterval();
}

/**
 * Checks credentials on connect_wifi button click.
 */
function checkCredentials()
{
	errorList = "";
	credsOk = true;
	
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	if (selectedSSID == "")
	{
		errorList += "<h4 class='rd'>SSID cannot be empty!</h4>";
		credsOk = false;
	}
	if (pwd == "")
	{
		errorList += "<h4 class='rd'>Password cannot be empty!</h4>";
		credsOk = false;
	}
	
	if (credsOk == false)
	{
		$("#wifi_connect_credentials_errors").html(errorList);
	}
	else
	{
		$("#wifi_connect_credentials_errors").html("");
		connectWifi();    
	}
}

/**
 * Shows the WiFi password if the box is checked.
 */
function showPassword()
{
	var x = document.getElementById("connect_pass");
	if (x.type === "password")
	{
		x.type = "text";
	}
	else
	{
		x.type = "password";
	}
}

function ESTADOS() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("counts").innerHTML = this.responseText;
        }
    };
    xhr.open("GET", "/ESTADOLED", true);
    xhr.send();
}


//proyecto final 


function checkCredentials2() {
    // Simulación de verificación de usuario y contraseña
    var username = document.getElementById('auth_user').value;
    var password = document.getElementById('auth_pass').value;

    // Lógica de autenticación
    if ((username === 'ian' && password === '1234') || (username === usuarioGlobal && password === contrasenaGlobal)) {
        // Autenticación exitosa
        document.getElementById('security_auth_credentials_errors').innerText = '';
        document.getElementById('security_auth_status').innerText = 'Autenticación exitosa';
        document.getElementById('toggleAlarm').style.display = 'block'; // Muestra el botón para activar/desactivar la alarma
        // Si las credenciales son válidas, muestra el botón de actualizar temperatura
        document.getElementById("updateTemperatureBtn").style.display = "block";
        document.getElementById("temperatureDisplay").style.display = "inline";
        // Muestra el nuevo botón
        document.getElementById("LH1").style.display = "block";
		document.getElementById("ESTADO1").style.display = "inline";
		document.getElementById("LE").style.display = "block";
		document.getElementById("ESTADO2").style.display = "inline";
		document.getElementById("NEVERA").style.display = "block";
		document.getElementById("ESTADO").style.display = "inline";
		document.getElementById("PERSIANASH1").style.display = "block";
		document.getElementById("ph1").style.display = "inline";
		document.getElementById("PERSIANASS").style.display = "block";
		document.getElementById("ps").style.display = "inline";
		document.getElementById("SIRENA").style.display = "block";
		document.getElementById("SIR").style.display = "inline";
    } else {
        // Autenticación fallida
        document.getElementById('security_auth_credentials_errors').innerText = 'Usuario o contraseña incorrectos';
        document.getElementById('security_auth_status').innerText = '';
        document.getElementById('toggleAlarm').style.display = 'none'; // Oculta el botón si la autenticación falla
        // Si las credenciales son válidas, muestra el botón de actualizar temperatura
        document.getElementById("updateTemperatureBtn").style.display = "none";
        // Oculta el nuevo botón en caso de autenticación fallida
        document.getElementById("LH1").style.display = "none";
		document.getElementById("LE").style.display = "none";
		document.getElementById("NEVERA").style.display = "none";
		document.getElementById("PERSIANASH1").style.display = "none";
		document.getElementById("PERSIANASS").style.display = "none";
		document.getElementById("SIRENA").style.display = "none";
    }
}

function toggleAlarm() {
    var alarmStatus = document.getElementById('alarm_status');
    var alarma = 0;

    // Lógica para activar/desactivar la alarma
    if (alarmStatus.innerText === 'Alarma activada') {
        alarmStatus.innerText = 'Alarma desactivada';
        alarma = 0;
    } else {
        alarmStatus.innerText = 'Alarma activada';
        alarma = 1;
    }

    // Enviar el estado de la alarma al servidor
    fetch('/activar_alarma', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: alarma, // Enviar directamente el valor como parte del cuerpo de la solicitud
    })
    .then(response => {
        if (!response.ok) {
            console.error('Error al enviar el estado de la alarma al servidor');
        }
    })
    .catch(error => {
        console.error('Error de red:', error);
    });
}

function updateTemperature() {
    var xhr = new XMLHttpRequest();

    xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
            if (xhr.status == 200) {
                // La solicitud fue exitosa
                var temperatura = xhr.responseText;

                // Actualiza el texto al lado del botón con el nuevo valor de temperatura
                document.getElementById("temperatureDisplay").innerText = "Temperatura: " + temperatura + " °C";
            } else {
                // Hubo un error en la solicitud
                console.error("Error al obtener la temperatura. Código de estado: " + xhr.status);
            }
        }
    };

    xhr.open("GET", "/enviar_temperatura", true);
    xhr.send();
}


function showPassword() {
	// Lógica para mostrar/ocultar la contraseña (puedes implementar esta función según tus necesidades)
}

var usuarioGlobal;
var contrasenaGlobal;

function updateCredentials() {
    var xhr = new XMLHttpRequest();

    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
            // Manejar la respuesta del servidor
            var nuevasCredenciales = xhr.responseText;

            // Separar las credenciales usando el delimitador
            var credencialesArray = nuevasCredenciales.split(';');

            // Asignar los valores a las variables globales
            usuarioGlobal = credencialesArray[0];
            contrasenaGlobal = credencialesArray[1];

            // Mostrar las nuevas credenciales en algún elemento HTML (por ejemplo, divs con id 'usuario' y 'contrasena')
            document.getElementById('usuario').innerText = usuarioGlobal;
            document.getElementById('contrasena').innerText = contrasenaGlobal;
        }
    };

    // Agrega aquí la lógica para actualizar las credenciales
    document.getElementById('actualizacion').innerText = 'Credenciales Actualizadas';

    xhr.open("GET", "/nuevas_credenciales", true);
    xhr.send();
}

// Variable para mantener el estado actual de la luz (inicializado en 1)
var estadoLuz = 0;

function botonhabitacion1() {
    // Obtener el valor opuesto al estado actual de la luz
    var valorLuz = estadoLuz === 1 ? 0 : 1;

    // Crear una instancia de XMLHttpRequest
    var xhr = new XMLHttpRequest();

    // Configurar la solicitud POST a la ruta "/activar_luz1"
    xhr.open("GET", "/activar_luz1", true);
    xhr.send();

    // Actualizar el estado de la luz para la próxima vez
    estadoLuz = valorLuz;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje');
    estadoElement.innerText = valorLuz === 1 ? 'Luz activada' : 'Luz desactivada';
}


var luzEntrada = 0;
function botonentrada() {
    // Obtener el valor opuesto al estado actual de la luz de entrada
    var valorEntrada = luzEntrada === 1 ? 0 : 1;

    // Crear una instancia de XMLHttpRequest
    var xhr = new XMLHttpRequest();

    // Configurar la solicitud POST a la ruta "/activar_entrada"
    xhr.open("GET", "/activar_luze", true);

    xhr.send();
    // Actualizar el estado de la luz de entrada para la próxima vez
    luzEntrada = valorEntrada;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje1');
    estadoElement.innerText = valorEntrada === 1 ? 'Luz de entrada activada' : 'Luz de entrada desactivada';
}

var estadoNevera = 0;
function botonnevera() {
    // Obtener el valor opuesto al estado actual de la nevera
    var valorNevera = estadoNevera === 1 ? 0 : 1;

    // Crear una instancia de XMLHttpRequest
    var xhr = new XMLHttpRequest();

    // Configurar la solicitud POST a la ruta "/activar_nevera"
    xhr.open("GET", "/activar_nevera", true);
    xhr.send();

    // Actualizar el estado de la nevera para la próxima vez
    estadoNevera = valorNevera;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje2');
    estadoElement.innerText = valorNevera === 1 ? 'Nevera activada' : 'Nevera desactivada';
}

var estadoPersianas = 0;
function botonpersianash1() {
    // Obtener el valor opuesto al estado actual de las persianas
    var valorPersianas = estadoPersianas === 1 ? 0 : 1;

    // Crear una instancia de XMLHttpRequest
    var xhr = new XMLHttpRequest();

    xhr.open("GET", "/activar_perh1", true);
    xhr.send();

    // Actualizar el estado de las persianas para la próxima vez
    estadoPersianas = valorPersianas;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje3');
    estadoElement.innerText = valorPersianas === 1 ? 'Persiana Arriba' : 'Persiana Abajo';
}

var estadoPersianas2 = 0;
function botonpersianass() {
    // Obtener el valor opuesto al estado actual de las persianas2
    var valorPersianas2 = estadoPersianas2 === 1 ? 0 : 1;

    // Crear una instancia de XMLHttpRequest
    var xhr = new XMLHttpRequest();

    xhr.open("GET", "/activar_pers", true);
    xhr.send();

    // Actualizar el estado de las persianas2 para la próxima vez
    estadoPersianas2 = valorPersianas2;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje4');
    estadoElement.innerText = valorPersianas2 === 1 ? 'Persiana Arriba' : 'Persiana Abajo';
}

var estadoSirena = 0;
function botonsirena() {
    // Obtener el valor opuesto al estado actual de la sirena
    var valorSirena = estadoSirena === 1 ? 0 : 1;

    var xhr = new XMLHttpRequest();

    xhr.open("GET", "/activar_sirena1", true);
    xhr.send();

    // Actualizar el estado de la sirena para la próxima vez
    estadoSirena = valorSirena;

    // Actualizar el elemento de estado según el valor enviado
    var estadoElement = document.getElementById('estadoMensaje5');
    estadoElement.innerText = valorSirena === 1 ? 'Sirena Activada' : 'Sirena Desactivada';
}








    










    


