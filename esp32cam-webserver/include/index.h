const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
    min-height: 250px;
    background: #12365fce;
    padding: 30px;
    box-sizing: border-box;
    color: #FFF;
    margin:20px;
    box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
    text-align: right;
}
</style>
<body>

<div class="card">
  <h1>Anteroposterior Acceleration</h1><br>
  <h2><span id="YACCEL">0</span></h2><br>
</div>


<script>
setInterval(function() {
  getData();
}, 200);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("YACCEL").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>
</body>
</html>
)rawliteral";