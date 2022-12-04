#define INDEX_HTML "<html>\n" \
	"<script type=\"text/javascript\">\n" \
	"function sendData(action) {\n" \
	"  var XHR = new XMLHttpRequest();\n" \
	"  XHR.addEventListener('load', function(event) {\n" \
	"      if (event.target.responseURL.endsWith(\"/rpc/getMagnetField\")) {\n" \
	"            var tmp = JSON.parse(event.target.response);\n" \
	"            var el = \"magnet\";\n" \
	"              document.getElementById(el).innerHTML = parseFloat(tmp.magnet) + \"T\";\n" \
	"              el = 'magnetMin';\n" \
	"              document.getElementById(el).innerHTML = parseFloat(tmp.magnetMin) + \"T\";\n" \
	"              el = 'magnetMax';\n" \
	"              document.getElementById(el).innerHTML = parseFloat(tmp.magnetMax) + \"T\";\n" \
	"              el = 'magnetMean';\n" \
	"              document.getElementById(el).innerHTML = parseFloat(tmp.magnetMean) + \"T\";\n" \
	"      } else if (event.target.responseURL.endsWith(\"rpc/getCounter\")) {\n" \
	"          var tmp = JSON.parse(event.target.response);\n" \
	"          var count = parseFloat(tmp.count);\n" \
	"          var duration = parseFloat(tmp.duration);\n" \
	"          var freq = parseFloat(tmp.freq);\n" \
	"          var el = \"counter\";\n" \
	"          var tmp1 = \"duration:\" + duration + \"s<br>count:\" + count + \"<br>frequence: \" + freq + \"/s<br> speed:\";\n" \
	"          var s = (freq * parseFloat(document.getElementById(\"counter_val_per_pulse\").value));\n" \
	"          var u = document.getElementById(\"counter_time_unit\").value;\n" \
	"          switch (u) {\n" \
	"              case \"m\":\n" \
	"                  s *= 60;\n" \
	"                  break;\n" \
	"              case \"h\":\n" \
	"                  s *= 3600;\n" \
	"                  break;\n" \
	"              default:\n" \
	"              case \"s\":\n" \
	"                  break;\n" \
	"          }\n" \
	"          tmp1 += s;\n" \
	"          tmp1 += document.getElementById(\"counter_val_unit\").value;\n" \
	"          tmp1 += \"/\" + u;\n" \
	"          document.getElementById(el).innerHTML = tmp1;\n" \
	"      } else if (event.target.responseURL.endsWith(\"/rpc/getAccel\")) {\n" \
	"            var tmp = JSON.parse(event.target.response);\n" \
	"          var el = \"accelRaw\";\n" \
	"          document.getElementById(el).innerHTML = JSON.stringify(tmp.accelN);\n" \
	"              el = \"accelRawI\";\n" \
	"          document.getElementById(el).innerHTML = JSON.stringify(tmp.accelI);\n" \
	"              el = \"accel\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.accel)+\"g<br>\"+(parseFloat(tmp.accel)*9.80655)+\"m/s^2\";\n" \
	"              el = \"speed\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.speed) + \"m/s<br>\" + (parseFloat(tmp.speed)*3.6) + \"km/h\";\n" \
	"\n" \
	"      } else {\n" \
	"          console.warn(event.target.response);\n" \
	"      }\n" \
	"  });\n" \
	"\n" \
	"  // Définissez ce qui arrive en cas d'erreur\n" \
	"  XHR.addEventListener('error', function(event) {\n" \
	"    alert(\"Oups! Quelque chose s'est mal passé.\");\n" \
	"  });\n" \
	"\n" \
	"  // Configurez la requête\n" \
	"  XHR.open('GET', action);\n" \
	"\n" \
	"  // Ajoutez l'en-tête HTTP requise pour requêtes POST de données de formulaire\n" \
	"//  XHR.setRequestHeader('Content-Type', 'plain/text');\n" \
	"\n" \
	"  // Finalement, envoyez les données.\n" \
	"  XHR.send();\n" \
	"}\n" \
	"function config_magneto_on_off () {\n" \
	"    var checkon = document.getElementById(\"switch_config\").checked;\n" \
	"    sendData(\"rpc/configMagneto?config=\" + (checkon?\"1\":\"0\"));\n" \
	"}\n" \
	"function init_accel_speed () {\n" \
	"    sendData(\"rpc/initAccelSpeed\");\n" \
	"}\n" \
	"function accel_on_off() {\n" \
	"    var checkon = document.getElementById(\"accel_speed_on\").checked;\n" \
	"    sendData(\"rpc/startAccelSpeed?start=\" + (checkon?\"1\":\"0\"));\n" \
	"}\n" \
	"function read_all () {\n" \
	"    sendData(\"rpc/getMagnetField\");\n" \
	"    sendData(\"rpc/getAccel\");\n" \
	"    sendData(\"rpc/getCounter\");\n" \
	"}\n" \
	"\n" \
	"document.onreadystatechange = function () {\n" \
	"  var state = document.readyState;\n" \
	"  if (state == 'interactive') {\n" \
	"  } else if (state == 'complete') {\n" \
	"   setInterval(read_all, 500);\n" \
	"  }\n" \
	"};\n" \
	"\n" \
	"</script>\n" \
	"<body>\n" \
	"  <h1>Welcome to Mongoose OS</h1>\n" \
	"    <fieldset>\n" \
	"      <legend>Magneto</legend>\n" \
	"    <fieldset>\n" \
	"      <legend>Config magneto</legend>\n" \
	"        <input type=\"checkbox\" id=\"switch_config\" name=\"switch_config\" onchange=\"config_magneto_on_off()\">\n" \
	"    </fieldset>\n" \
	"      <div id=\"magnet\" name=\"magnet\">\n" \
	"      </div>\n" \
	"      <div id=\"magnetMean\" name=\"magnetMean\">\n" \
	"      </div>\n" \
	"      <div id=\"magnetMax\" name=\"magnetMax\">\n" \
	"      </div>\n" \
	"      <div id=\"magnetMin\" name=\"magnetMin\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"    <fieldset>\n" \
	"      <legend>Accel and Speed</legend>\n" \
	"        <fieldset>\n" \
	"            <legend>Init accel speed</legend>\n" \
	"            Start accel module: <input type=\"checkbox\" id=\"accel_speed_on\" name=\"accel_speed_on\" onchange=\"accel_on_off()\"><br>\n" \
	"            <input type=\"button\" id=\"button_init_accel_speed\" name=\"button_init_accel_speed\" onclick=\"init_accel_speed()\" value=\"Reset\">\n" \
	"        </fieldset>\n" \
	"      Speed :<span id=\"speed\" name=\"speed\"></span><br>\n" \
	"      Accel: <span id=\"accel\" name=\"accel\"></span><br>\n" \
	"      Accel: <span id=\"accelRaw\" name=\"accelRaw\"></span><br>\n" \
	"      Accel initial: <span id=\"accelRawI\" name=\"accelRawI\"></span>\n" \
	"    </fieldset>\n" \
	"    <fieldset>\n" \
	"      <legend>Count</legend>\n" \
	"        <fieldset>\n" \
	"            <legend>Parametrage vitesse par roue codeuse</legend>\n" \
	"            Longueur ou angle (ou autre) mesuree entre chaque pulsation: <input type=\"text\" id=\"counter_val_per_pulse\" name=\"counter_val_per_pulse\" value=\"1\">\n" \
	"            <br>\n" \
	"            Unite de la valeur mesuree: <input type=\"text\" id=\"counter_val_unit\" name=\"counter_val_unit\" value=\"\">\n" \
	"            <br>\n" \
	"            Unite de temps souhaitee:\n" \
	"            <select id=\"counter_time_unit\" name=\"counter_time_unit\">\n" \
	"                <option value=\"s\" selected>seconde</option>\n" \
	"                <option value=\"m\">minute</option>\n" \
	"                <option value=\"h\">heure</option>\n" \
	"            </select>\n" \
	"        </fieldset>\n" \
	"      <div id=\"counter\" name=\"counter\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"\n" \
	"</body>\n" \
	"</html>\n" \
	""