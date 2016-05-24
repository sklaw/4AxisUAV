// import libraries
import java.awt.Frame;
import java.awt.BorderLayout;
import controlP5.*; // http://www.sojamo.de/libraries/controlP5/
import processing.serial.*;
import java.util.Scanner;

/* SETTINGS BEGIN */

// Serial port to connect to
String serialPortName = "/dev/ttyACM0";



/* SETTINGS END */

Serial serialPort; // Serial port object

// interface stuff
ControlP5 cp5;

// Settings for the plotter are saved in this file
JSONObject plotterConfigJSON;

// plots
Graph LineGraph = new Graph(200, 70, 600, 150, color (20, 20, 200));
Graph lower_LineGraph = new Graph(200, 350, 600,150, color (20, 20, 200));

float[][] lineGraphValues = new float[9][100];
float[][] lower_lineGraphValues = new float[4][100];

float[] lineGraphSampleNumbers = new float[100];
color[] graphColors = new color[9];

// helper for saving the executing path
String topSketchPath = "";

void setup() {
  frame.setTitle("Realtime plotter");
  size(900, 700);




  // set line graph colors
  graphColors[0] = color(131, 255, 20);
  graphColors[1] = color(232, 158, 12);
  graphColors[2] = color(255, 0, 0);
  graphColors[3] = color(62, 12, 232);
  graphColors[4] = color(13, 255, 243);
  graphColors[5] = color(200, 46, 232);
  graphColors[6] = color(0, 0, 0);
  graphColors[7] = color(255, 255, 0);
  graphColors[8] = color(255, 165, 0);


  // settings save file
  topSketchPath = sketchPath();
  plotterConfigJSON = loadJSONObject(topSketchPath+"/plotter_config.json");

  // gui
  cp5 = new ControlP5(this);

  // init charts
  setChartSettings();

  // build x axis values for the line graph
  for (int i=0; i<lineGraphValues.length; i++) {
    for (int k=0; k<lineGraphValues[0].length; k++) {
      lineGraphValues[i][k] = 0;
      if (i==0)
        lineGraphSampleNumbers[k] = k;
    }
  }




  // build the gui
  int x = 13;
  int y = 0;
  int y_pad = 40;
  cp5.addTextlabel("label").setText("on/off").setPosition(x=13, y).setColor(0);
  cp5.addToggle("lgVisible1").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible1"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[0]);
  cp5.addTextlabel("label1").setText("ax").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible2").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible2"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[1]);
  cp5.addTextlabel("label2").setText("ay").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible3").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible3"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[2]);
  cp5.addTextlabel("label3").setText("az").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible4").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible4"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[3]);
  cp5.addTextlabel("label4").setText("gx").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible5").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible5"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[4]);
  cp5.addTextlabel("label5").setText("gy").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible6").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible6"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[5]);
  cp5.addTextlabel("label6").setText("gz").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible7").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible7"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[6]);
  cp5.addTextlabel("label7").setText("yaw").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible8").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible8"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[7]);
  cp5.addTextlabel("label8").setText("pitch").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lgVisible9").setPosition(x, y=y+y_pad).setValue(int(getPlotterConfigString("lgVisible9"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[8]);
  cp5.addTextlabel("label9").setText("roll").setPosition(x+50, y).setColor(0);

  x = 13;
  y = 360;
  cp5.addToggle("lower_lgVisible1").setPosition(x, y=y+40).setValue(int(getPlotterConfigString("lower_lgVisible1"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[0]);
  cp5.addTextlabel("lower_label1").setText("m0").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lower_lgVisible2").setPosition(x, y=y+40).setValue(int(getPlotterConfigString("lower_lgVisible2"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[1]);
  cp5.addTextlabel("lower_label2").setText("m1").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lower_lgVisible3").setPosition(x, y=y+40).setValue(int(getPlotterConfigString("lower_lgVisible3"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[2]);
  cp5.addTextlabel("lower_label3").setText("m2").setPosition(x+50, y).setColor(0);
  cp5.addToggle("lower_lgVisible4").setPosition(x, y=y+40).setValue(int(getPlotterConfigString("lower_lgVisible4"))).setMode(ControlP5.SWITCH).setColorActive(graphColors[3]);
  cp5.addTextlabel("lower_label4").setText("m3").setPosition(x+50, y).setColor(0);

  PFont font = createFont("arial", 20);
  cp5.addTextfield("textInput_1")
    .setPosition(13, 600)
      .setSize(500, 40)
        .setFont(font)
          .setFocus(true)
            .setColor(color(255, 0, 0))
              ;


  LineGraph.DrawAxis();
  for (int i=0; i<lineGraphValues.length; i++) {
    LineGraph.GraphColor = graphColors[i];
    if (int(getPlotterConfigString("lgVisible"+(i+1))) == 1)
      LineGraph.LineGraph(lineGraphSampleNumbers, lineGraphValues[i]);
  }

  lower_LineGraph.DrawAxis();
  for (int i=0; i<lower_lineGraphValues.length; i++) {
    lower_LineGraph.GraphColor = graphColors[i];
    if (int(getPlotterConfigString("lower_lgVisible"+(i+1))) == 1)
      lower_LineGraph.LineGraph(lineGraphSampleNumbers, lower_lineGraphValues[i]);
  }
  
    // start serial communication

  //String serialPortName = Serial.list()[3];
  serialPort = new Serial(this, serialPortName, 115200);
}

byte[] inBuffer = new byte[100]; // holds serial message
int i = 0; // loop variable
int time_to_draw = 0;
int ax, ay, az, gx, gy, gz;

void draw() {
  /* Read serial and update values */
  if (serialPort.available() > 0) {
    String now_str = null;
    now_str = serialPort.readStringUntil(';');

    if (now_str == null) {
      return;
    }

    if (time_to_draw == 1) {
      time_to_draw = 0;
      ;
    } else if (now_str.equals("PLOT DATA;")) {
      time_to_draw = 1;
      return;
    } else {
      println(now_str);
      return;
    }


    String[] nums = split(now_str.substring(0, now_str.length()-1), ' ');

    // build the arrays for bar charts and line graphs
    for (i=0; i<9; i++) {
      // update line graph
      try {
        if (i<lineGraphValues.length) {
          for (int k=0; k<lineGraphValues[i].length-1; k++) {
            lineGraphValues[i][k] = lineGraphValues[i][k+1];
          }

          lineGraphValues[i][lineGraphValues[i].length-1] = float(nums[i]);
        }
      }
      catch (Exception e) {
      }
    }

    for (i=0; i<4; i++) {
      // update line graph
      try {
        if (i<lower_lineGraphValues.length) {
          for (int k=0; k<lower_lineGraphValues[i].length-1; k++) {
            lower_lineGraphValues[i][k] = lower_lineGraphValues[i][k+1];
          }

          lower_lineGraphValues[i][lower_lineGraphValues[i].length-1] = float(nums[i+9]);
        }
      }
      catch (Exception e) {
      }
    }




    background(255); 

    // draw the line graphs
    LineGraph.DrawAxis();
    for (int i=0; i<lineGraphValues.length; i++) {
      LineGraph.GraphColor = graphColors[i];
      if (int(getPlotterConfigString("lgVisible"+(i+1))) == 1)
        LineGraph.LineGraph(lineGraphSampleNumbers, lineGraphValues[i]);
    }

    lower_LineGraph.DrawAxis();
    for (int i=0; i<lower_lineGraphValues.length; i++) {
      lower_LineGraph.GraphColor = graphColors[i];
      if (int(getPlotterConfigString("lower_lgVisible"+(i+1))) == 1)
        lower_LineGraph.LineGraph(lineGraphSampleNumbers, lower_lineGraphValues[i]);
    }
  }
}

// called each time the chart settings are changed by the user 
void setChartSettings() {


  LineGraph.xLabel=" Samples ";
  LineGraph.yLabel="Value";
  LineGraph.Title="";  
  LineGraph.xDiv=20;  
  LineGraph.xMax=0; 
  LineGraph.xMin=-100;  
  LineGraph.yMax=2000; 
  LineGraph.yMin=-2000;

  lower_LineGraph.xLabel=" Samples ";
  lower_LineGraph.yLabel="Value";
  lower_LineGraph.Title="";  
  lower_LineGraph.xDiv=20;  
  lower_LineGraph.xMax=0; 
  lower_LineGraph.xMin=-100;  
  lower_LineGraph.yMax=2000; 
  lower_LineGraph.yMin=500;
}

// handle gui actions
void controlEvent(ControlEvent theEvent) {
  if (theEvent.isAssignableFrom(Toggle.class) || theEvent.isAssignableFrom(Button.class)) {
    String parameter = theEvent.getName();
    String value = "";
    if (theEvent.isAssignableFrom(Textfield.class))
      value = theEvent.getStringValue();
    else if (theEvent.isAssignableFrom(Toggle.class) || theEvent.isAssignableFrom(Button.class))
      value = theEvent.getValue()+"";

    plotterConfigJSON.setString(parameter, value);
    saveJSONObject(plotterConfigJSON, topSketchPath+"/plotter_config.json");
    setChartSettings();
  }
  else if (theEvent.isAssignableFrom(Textfield.class)) {
      serialPort.write(theEvent.getStringValue());
  }
  
}

// get gui settings from settings file
String getPlotterConfigString(String id) {
  String r = "";
  try {
    r = plotterConfigJSON.getString(id);
  } 
  catch (Exception e) {
    r = "";
  }
  return r;
}