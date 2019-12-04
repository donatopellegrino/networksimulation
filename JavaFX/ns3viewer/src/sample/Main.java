package sample;

import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Pos;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.layout.*;
import javafx.scene.text.Text;
import javafx.stage.Stage;

import java.util.ArrayList;


public class Main extends Application {

    ArrayList<Node> nodeList = new ArrayList<Node>();
    Node temp;
    @Override
    public void start(Stage primaryStage) throws Exception{
        primaryStage.setTitle("Topology Creator");
        Text AS1 = new Text("AS1");
        Text AS2 = new Text("AS2");
        Text AS3 = new Text("AS3");
        Text AS4 = new Text("AS4");
        Text AS5 = new Text("AS5");
        Text AS6 = new Text("AS6");
        Text AS7 = new Text("AS7");
        Text AS8 = new Text("AS8");
        Text AS9 = new Text("AS9");

        GridPane gridpane = new GridPane();
        gridpane.setMinSize(1000,1000);
        gridpane.setHgap(10);
        RowConstraints buttonRow = new RowConstraints();
        RowConstraints mainScreen = new RowConstraints();
        mainScreen.setPercentHeight(90);
        GridPane mainPane = new GridPane();





        HBox buttonBox = new HBox();
        TextField asField = new TextField("Set AS");
        Button addNode = new Button("Add Node");
        addNode.setOnAction(new EventHandler() {
            @Override
            public void handle(Event event) {
                System.out.println("Adding New node");

                temp = new Node(1, Integer.parseInt(asField.getText()));
                nodeList.add(temp);
                temp.setCenterX(100.0f);
                temp.setCenterY(100.0f);
                temp.setRadius(50.0f);
                gridpane.add(mainPane, 0, 1);
            }
        });
        Button deleteNode = new Button("Delete Node");
        gridpane.getRowConstraints().addAll(buttonRow,mainScreen);
        buttonBox.getChildren().addAll(asField,addNode,deleteNode);
        buttonBox.setSpacing(20);

        gridpane.add(buttonBox,0,0);
        gridpane.add(mainPane, 0,1);
        primaryStage.setScene(new Scene(gridpane));
        primaryStage.show();
    }


    public static void main(String[] args) {
        launch(args);
    }
}
