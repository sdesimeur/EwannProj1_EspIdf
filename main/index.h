#define INDEX_HTML "<html>\n" \
	"<script type=\"text/javascript\">\n" \
	"function sendData(action, dataJSON) {\n" \
	"  var XHR = new XMLHttpRequest();\n" \
	"  var dataTxt = JSON.stringify(dataJSON);\n" \
	"  XHR.addEventListener('load', function(event) {\n" \
	"      if (event.target.responseURL.endsWith(\"rpc/getSpeed\")) {\n" \
	"            var tmp = parseFloat(event.target.response);\n" \
	"            var tmp1 = 3.6*tmp;\n" \
	"          document.getElementById('speed').innerHTML = tmp + \"m/s<br>\" + tmp1 + \"km/h\";\n" \
	"      } else if (event.target.responseURL.endsWith(\"rpc/initAccelSpeed\")) {\n" \
	"          console.log(JSON.stringify(event.target.response));\n" \
	"      } else if ((new RegExp(\"/rpc/getMagnetField\\?\")).test(event.target.responseURL)) {\n" \
	"            var tmp = parseFloat(event.target.response);\n" \
	"          var el = \"magnet\";\n" \
	"          if (event.target.responseURL.endsWith(\"n\")) {\n" \
	"              el = 'magnet';\n" \
	"          } else if (event.target.responseURL.endsWith(\"m\")) {\n" \
	"              el = 'magnetMin';\n" \
	"          } else if (event.target.responseURL.endsWith(\"M\")) {\n" \
	"              el = 'magnetMax';\n" \
	"          } else if (event.target.responseURL.endsWith(\"2\")) {\n" \
	"              el = 'magnetMean';\n" \
	"          }\n" \
	"          document.getElementById(el).innerHTML = tmp + \"T\";\n" \
	"      } else if (event.target.responseURL.endsWith(\"rpc/getAccel\")) {\n" \
	"            var tmp = parseFloat(event.target.response);\n" \
	"            var tmp1 = tmp * 9.80655;\n" \
	"          document.getElementById('accel').innerHTML = tmp+\"g<br>\"+tmp1+\"m/s^2\";\n" \
	"      } else if (event.target.responseURL.endsWith(\"rpc/getCounter\")) {\n" \
	"          var tmp = JSON.parse(event.target.response);\n" \
	"          var count = parseFloat(tmp.count);\n" \
	"          var duration = parseFloat(tmp.duration);\n" \
	"          var el = \"counter\";\n" \
	"          var tmp1 = \"duration:\" + duration + \"s<br>count:\" + count \n" \
	"          document.getElementById(el).innerHTML = tmp1;\n" \
	"\n" \
	"      } else if (event.target.responseURL.endsWith(\"rpc/getRawAccel\")) {\n" \
	"      } else if ((new RegExp(\"/rpc/getRawAccl\\?\")).test(event.target.responseURL)) {\n" \
	"            var tmp = (event.target.response);\n" \
	"          var el = \"accelRaw\";\n" \
	"          if (event.target.responseURL.endsWith(\"n\")) {\n" \
	"              el = \"accelRaw\";\n" \
	"          } else if (event.target.responseURL.endsWith(\"i\")) {\n" \
	"              el = \"accelRawI\";\n" \
	"          }\n" \
	"          document.getElementById(el).innerHTML = tmp;\n" \
	"      }\n" \
	"/*\n" \
	"      if (event.target.responseURL.endsWith(\"rpc/FS.Get\")) {\n" \
	"          if (event.target.response.includes('\"data\":')) {\n" \
	"            var tmp = atob(JSON.parse(event.target.response).data);\n" \
	"            if (tmp.startsWith(\"{\")) {\n" \
	"                document.getElementById('filetxt').value = JSON.stringify(JSON.parse(tmp), undefined, 4);\n" \
	"            } else {\n" \
	"                document.getElementById('filetxt').value = tmp;\n" \
	"            }\n" \
	"          } else {\n" \
	"              alert (event.target.response);\n" \
	"          }\n" \
	"      }\n" \
	"*/\n" \
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
	"  XHR.setRequestHeader('Content-Type', 'plain/text');\n" \
	"\n" \
	"  // Finalement, envoyez les données.\n" \
	"  XHR.send(dataTxt);\n" \
	"}\n" \
	"function config_magneto_on_off () {\n" \
	"    var checkon = document.getElementById(\"switch_config\").checked;\n" \
	"    var dataJSON = {};\n" \
	"    sendData(\"rpc/configMagneto?config=\" + (checkon?\"1\":\"0\"), dataJSON);\n" \
	"}\n" \
	"function init_accel_speed () {\n" \
	"    var dataJSON = {};\n" \
	"    sendData(\"rpc/initAccelSpeed\",dataJSON);\n" \
	"}\n" \
	"\n" \
	"function read_all () {\n" \
	"    var dataJSON = {};\n" \
	"    sendData(\"rpc/getSpeed\",dataJSON);\n" \
	"    sendData(\"rpc/getMagnetField?w=m\",dataJSON);\n" \
	"    sendData(\"rpc/getMagnetField?w=M\",dataJSON);\n" \
	"    sendData(\"rpc/getMagnetField?w=2\",dataJSON);\n" \
	"    sendData(\"rpc/getMagnetField?w=n\",dataJSON);\n" \
	"    sendData(\"rpc/getAccel\",dataJSON);\n" \
	"    sendData(\"rpc/getCounter\",dataJSON);\n" \
	"    sendData(\"rpc/getRawAccel?w=n\",dataJSON);\n" \
	"    sendData(\"rpc/getRawAccel?w=i\",dataJSON);\n" \
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
	"      <div id=\"magnetMean\" name=\"magnetMean\">\n" \
	"      <div id=\"magnetMax\" name=\"magnetMax\">\n" \
	"      <div id=\"magnetMin\" name=\"magnetMin\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"    <fieldset>\n" \
	"      <legend>Speed</legend>\n" \
	"    <fieldset>\n" \
	"      <legend>Init accel speed</legend>\n" \
	"      <input type=\"button\" id=\"button_init_accel_speed\" name=\"button_init_accel_speed\" onclick=\"init_accel_speed()\" value=\"INIT\">\n" \
	"    </fieldset>\n" \
	"      <div id=\"speed\" name=\"speed\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"    <fieldset>\n" \
	"      <legend>Accel</legend>\n" \
	"      <div id=\"accel\" name=\"accel\">\n" \
	"      </div>\n" \
	"      <div id=\"accelRaw\" name=\"accelRaw\">\n" \
	"      </div>\n" \
	"      <div id=\"accelRawI\" name=\"accelRawI\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"    <fieldset>\n" \
	"      <legend>Count</legend>\n" \
	"      <div id=\"counter\" name=\"counter\">\n" \
	"      </div>\n" \
	"    </fieldset>\n" \
	"\n" \
	"</body>\n" \
	"</html>\n" \
	""