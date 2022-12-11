#define INDEX_HTML "<html>\n" \
	"<script type=\"text/javascript\">\n" \
	"function sendData(action) {\n" \
	"  var XHR = new XMLHttpRequest();\n" \
	"  XHR.addEventListener('load', function(event) {\n" \
	"      if (event.target.responseURL.endsWith(\"/getAllValues\")) {\n" \
	"          var tmp = JSON.parse(event.target.response);\n" \
	"          var el = \"magnet\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.magnet.val) + \"T\";\n" \
	"          el = 'magnetMin';\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.magnet.Min) + \"T\";\n" \
	"          el = 'magnetMax';\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.magnet.Max) + \"T\";\n" \
	"          el = 'magnetMean';\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.magnet.Mean) + \"T\";\n" \
	"          \n" \
	"          var count = parseFloat(tmp.counter.val);\n" \
	"          var duration = parseFloat(tmp.counter.duration);\n" \
	"          var freq = parseFloat(tmp.counter.freq);\n" \
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
	"          el = \"counter\";\n" \
	"          document.getElementById(el).innerHTML = tmp1;\n" \
	"          \n" \
	"          var el = \"accelRaw\";\n" \
	"          document.getElementById(el).innerHTML = JSON.stringify(tmp.accel.N);\n" \
	"              el = \"accelMaxWG\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.accel.MaxWG);\n" \
	"              el = \"accelRawI\";\n" \
	"          document.getElementById(el).innerHTML = JSON.stringify(tmp.accel.I);\n" \
	"              el = \"accel\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.accel.val)+\"g<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\"+(parseFloat(tmp.accel.val)*9.80655)+\"m/s^2\";\n" \
	"              el = \"accelWG\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.accel.WG)+\"g<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\"+(parseFloat(tmp.accel.WG)*9.80655)+\"m/s^2\";\n" \
	"              el = \"speed\";\n" \
	"          document.getElementById(el).innerHTML = parseFloat(tmp.accel.speed) + \"m/s<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\" + (parseFloat(tmp.accel.speed)*3.6) + \"km/h\";\n" \
	"\n" \
	"      } else {\n" \
	"          console.warn(event.target.response);\n" \
	"      }\n" \
	"  });\n" \
	"\n" \
	"  // Définissez ce qui arrive en cas d'erreur\n" \
	"  XHR.addEventListener('error', function(event) {\n" \
	"    //console.warn(\"Oups! Quelque chose s'est mal passé.\");\n" \
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
	"    sendData(\"/command?initMagnetoField=\" + (checkon?\"1\":\"0\"));\n" \
	"}\n" \
	"function init_accel_speed () {\n" \
	"    sendData(\"/command?initAccelSpeed=1\");\n" \
	"}\n" \
	"function accel_on() {\n" \
	"    sendData(\"/command?startAccelSpeed=1\");\n" \
	"    document.getElementById(\"accel_speed_on\").hidden=true;\n" \
	"    document.getElementById(\"checkD8\").hidden=true;\n" \
	"}\n" \
	"function D7_on_off() {\n" \
	"    var checkon = document.getElementById(\"checkD7\").checked;\n" \
	"    sendData(\"/command?gpio=13&level=\" + (checkon?\"1\":\"0\"));\n" \
	"}\n" \
	"function D8_on_off() {\n" \
	"    var checkon = document.getElementById(\"checkD8\").checked;\n" \
	"    document.getElementById(\"allAccelView\").hidden=true;\n" \
	"    sendData(\"/command?gpio=15&level=\" + (checkon?\"1\":\"0\"));\n" \
	"}\n" \
	"\n" \
	"document.onreadystatechange = function () {\n" \
	"  var state = document.readyState;\n" \
	"  if (state == 'interactive') {\n" \
	"  } else if (state == 'complete') {\n" \
	"      setInterval(() => {\n" \
	"        sendData(\"/getAllValues\");\n" \
	"      }, 500);\n" \
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
	"      Value: <span id=\"magnet\" name=\"magnet\">\n" \
	"      </span><br>\n" \
	"      Mean: <span id=\"magnetMean\" name=\"magnetMean\">\n" \
	"      </span><br>\n" \
	"      Max: <span id=\"magnetMax\" name=\"magnetMax\">\n" \
	"      </span><br>\n" \
	"      Min: <span id=\"magnetMin\" name=\"magnetMin\">\n" \
	"      </span>\n" \
	"    </fieldset>\n" \
	"    <fieldset id=\"allAccelView\">\n" \
	"      <legend>Accel and Speed</legend>\n" \
	"        <fieldset>\n" \
	"            <legend>Init accel speed</legend>\n" \
	"            <input type=\"button\" id=\"accel_speed_on\" name=\"accel_speed_on\" onclick=\"accel_on()\" value=\"Start accel module\"><br>\n" \
	"            <input type=\"button\" id=\"button_init_accel_speed\" name=\"button_init_accel_speed\" onclick=\"init_accel_speed()\" value=\"Reset\">\n" \
	"        </fieldset>\n" \
	"      Speed :<span id=\"speed\" name=\"speed\"></span><br>\n" \
	"      Accel: <span id=\"accel\" name=\"accel\"></span><br>\n" \
	"      Accel vector: <span id=\"accelRaw\" name=\"accelRaw\"></span><br>\n" \
	"      AccelWG: <span id=\"accelWG\" name=\"accelWG\"></span><br>\n" \
	"      Accel max.: <span id=\"accelMaxWG\" name=\"accelMaxWG\"></span><br>\n" \
	"      Accel vector initial: <span id=\"accelRawI\" name=\"accelRawI\"></span>\n" \
	"    </fieldset>\n" \
	"    <fieldset id=\"countView\">\n" \
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
	"        <fieldset>\n" \
	"            <legend>Demarrage des leds</legend>\n" \
	"            D7: <input type=\"checkbox\" id=\"checkD7\" onchange=\"D7_on_off()\"><br> \n" \
	"            D8: <input type=\"checkbox\" id=\"checkD8\" onchange=\"D8_on_off()\"><br> \n" \
	"        </fieldset>\n" \
	"      <div id=\"counter\" name=\"counter\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"\n" \
	"</body>\n" \
	"</html>\n" \
	""