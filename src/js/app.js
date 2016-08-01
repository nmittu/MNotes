var id = localStorage.getItem("UID");
var apiKey = localStorage.getItem("APIKEY");
var cach = null;
if(localStorage.getItem("CACH")){
	//cach = JSON.parse(localStorage.getItem("CACH"));
}

function hostReachable() {
	//return false;
  // Handle IE and more capable browsers
  var xhr = new XMLHttpRequest();

  // Open new request as a HEAD to the root hostname with a random param to bust the cache
	xhr.open( "HEAD", "https://pebbleconf.mittudev.com"+ "/?rand=" + Math.floor((1 + Math.random()) * 0x10000), false);

  // Issue request and handle response
  try {
    xhr.send();
    return ( xhr.status >= 200 && (xhr.status < 300 || xhr.status === 304) );
  } catch (error) {
    return false;
  }

}

function getIndex(){
	console.log("ind");
	console.log(hostReachable());
	var dictionary;
	if(id === null || apiKey === null){
		dictionary = {
			"TYPE": 2
		};
		Pebble.sendAppMessage(dictionary,
			function(e){
				console.log('Index Sent');
			},
			function(e){
				console.log('Error Sending Weather To Pebble!');
			}
		);
		return;
	}
	if(hostReachable()){
		var xhr = new XMLHttpRequest();
		xhr.onload = function(){
			var json = JSON.parse(xhr.responseText);
			var titles = json.notes;

			dictionary = {
				"TYPE": 0,
				"ROWS": titles.length
			};
			for(var i = 0; i < titles.length; i++){
				dictionary[i+100] = titles[i].title;
			}
			//console.log(dictionary[titles[0].title]);

			Pebble.sendAppMessage(dictionary,
				function(e){
					console.log('Index Sent');
				},
				function(e){
					console.log('Error Sending Weather To Pebble!');
				}
			);
		};
		xhr.open("POST", "https://note.mittudev.com/api.php");
		xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xhr.send("id="+id+"&apikey="+apiKey);
	}else{
		if(cach){
			dictionary = {
					"TYPE": 0,
					"ROWS": Object.keys(cach).length
				};
			
			for(var i = 0; i<Object.keys(cach).length; i++){
				dictionary[i+100] = Object.keys(cach)[i];
			}
			
			Pebble.sendAppMessage(dictionary,
				function(e){
					console.log('Index Sent');
				},
				function(e){
					console.log('Error Sending Weather To Pebble!');
				}
			);
		}
	}
}

Pebble.addEventListener('ready',
  function(e){
    console.log('PebbleKit JS ready!');
		
		getIndex();
	}
);

Pebble.addEventListener('appmessage',
  function(e){
		console.log('AppMessage recived!');
		var title;
		var xhr;
		
		if(e.payload.TYPE === 0){
			if(hostReachable()){
				title = e.payload.TITLE;

				xhr = new XMLHttpRequest();
				xhr.onload = function(){
					var dictionary = {
						"TYPE": 1,
						"TITLE": title,
						"CONTENT": xhr.responseText
					};

					Pebble.sendAppMessage(dictionary,
						function(e){
							console.log(title+' Sent');
						},
						function(e){
							console.log('Error Sending Weather To Pebble!');
						}
					);
				};

				xhr.open("POST", "https://note.mittudev.com/read.php");
				xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
				xhr.send("id="+id+"&apikey="+apiKey+"&title="+title);
			}else{
				console.log(JSON.stringify(cach));
				if(cach[e.payload.TITLE]){
					var dictionary = {
						"TYPE": 1,
						"TITLE": e.payload.TITLE,
						"CONTENT": cach[e.payload.TITLE]
					};
					
					Pebble.sendAppMessage(dictionary,
						function(e){
							console.log(title+' Sent');
						},
						function(e){
							console.log('Error Sending Weather To Pebble!');
						}
					);
				}
			}
		}else if(e.payload.TYPE === 1){
			getIndex();
		}else if(e.payload.TYPE === 2){			
			xhr = new XMLHttpRequest();
			xhr.onload = function(){
				var dictionary = {
					"TYPE": 3
				};
				
				console.log(e.payload.TITLE);
				
				if(cach){
					cach[e.payload.TITLE] = xhr.responseText;
				}else{
					cach = {};
					cach[e.payload.TITLE] = xhr.responseText;
				}
				
				localStorage.setItem("CACH", JSON.stringify(cach));
				
				Pebble.sendAppMessage(dictionary,
					function(e){
						console.log('Sent');
					},
					function(e){
						console.log('Error Sending Weather To Pebble!');
					}
				);
			};
			
			xhr.open("POST", "https://note.mittudev.com/read.php");
			xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			xhr.send("id="+id+"&apikey="+apiKey+"&title="+e.payload.TITLE);
		}
	}
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://pebbleconf.mittudev.com/mnotes.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config_data = JSON.parse(decodeURIComponent(e.response));
  console.log('Config window returned: ', JSON.stringify(config_data));

  localStorage.setItem("UID", config_data.UID);
	localStorage.setItem("APIKEY", config_data.APIKEY);
	
	var dictionary ={
		"TYPE": 4,
		"P_RED": parseInt(config_data.pColor.substring(2, 4), 16),
		"P_GREEN": parseInt(config_data.pColor.substring(4, 6), 16),
		"P_BLUE": parseInt(config_data.pColor.substring(6), 16),
		"S_RED": parseInt(config_data.sColor.substring(2, 4), 16),
		"S_GREEN": parseInt(config_data.sColor.substring(4, 6), 16),
		"S_BLUE": parseInt(config_data.sColor.substring(6), 16),
		"SHADOWS": config_data.shadows,
		"FONT": config_data.font
	};
	
	Pebble.sendAppMessage(dictionary,
		function(e){
			console.log('Sent');
		},
		function(e){
			console.log('Error Sending Weather To Pebble!');
		}
	);
	
	if(config_data.clearCach){
		localStorage.removeItem("CACH");
		cach = null;
	}
	
	id = config_data.UID;
	apiKey = config_data.APIKEY;
	
	getIndex();
});