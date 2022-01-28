const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>

<head>
  <title>Ntc Data Graph | Stoper</title>
  <script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>  
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }

  /* Data Table Styling */
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
  }

  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }

  #dataTable tr:nth-child(even){background-color: #f2f2f2;}

  #dataTable tr:hover {background-color: #ddd;}

  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: left;
    background-color: #ff0000;
    color: white;
  }
  </style>
</head>

<body>
    <div style="text-align:center;"><b></b><br></div>
    <div class="chart-container" position: relative; height:350px; width:100%">
        <canvas id="Chart" width="400" height="400"></canvas>
    </div>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>NTC Temp (&deg;C)</th><th></tr>
  </table>
</div>
<br>
<br>  

<script>
var ADCvalues = [];
var Tvalues = [];
var Axvalues = [];
var Ayvalues = [];
var Azvalues = [];
var Gxvalues = [];
var Gyvalues = [];
var Gzvalues = [];
var timeStamp = [];
function showGraph()
{
    var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [{
                label: "NTC Temp",
                fill: false,  //Try with true
                backgroundColor: 'rgba( 243,18, 156 , 1)', //Dot marker color
                borderColor: 'rgba( 243, 18, 156 , 1)', //Graph Line Color
                                  pointRadius:0,
                                   borderWidth:1,

                data: ADCvalues,
            }],
        },
        options: {
          animation:{
            duration: 0
            },
            title: {
                    display: true,
                    text: "Data Graphs"
                },
            maintainAspectRatio: false,
            elements: {
            line: {
                    tension: 0.2 //Smoothening (Curved) of data lines
                }
            },
            scales: {
                    yAxes: [{
                      display: true,
                        ticks: {
                            beginAtZero:false
                        }
                    }],
                    xAxes:[{
                      display: true
                      }]
            }
        }
    });

}

//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString());
};

setInterval(function() {
  getData();
}, 1000); //1000 milliseconds update rate
 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var txt = this.responseText;
  var obj = JSON.parse(txt);
      ADCvalues.push(obj.ADC);
      Tvalues.push(obj.Temperature);
      timeStamp.push(time);
      showGraph();  //Update Graphs
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1);
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);

    cell1.innerHTML = time;
    cell2.innerHTML = obj.ADC;
    cell3.innerHTML = obj.Temperature;

    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
    
</script>
</body>

</html>

)=====";
