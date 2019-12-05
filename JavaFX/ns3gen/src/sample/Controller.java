package sample;

import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.scene.shape.Circle;
import javafx.scene.text.Text;

import java.util.ArrayList;
import java.util.List;

public class Controller {

    static int id = 1;
    double orgSceneX, orgSceneY;
    double orgTranslateX, orgTranslateY;

    List<Node> nodeList = new ArrayList<>();
    int nodeSelected = -1;

    @FXML public TextField asTxtField;
    @FXML public Text infoField;
    @FXML public Pane mainPane;

    @FXML protected void addNewNode() {
        try {
            Node node = new Node(id, Integer.parseInt(asTxtField.getText()));
            System.out.println(asTxtField.getText());
            EventHandler<MouseEvent> nodeOnMousePressedEventHandler =
                    new EventHandler<MouseEvent>() {
                        @Override
                        public void handle(MouseEvent t) {
                            orgSceneX = t.getSceneX();
                            orgSceneY = t.getSceneY();
                            orgTranslateX = ((Circle)(t.getSource())).getTranslateX();
                            orgTranslateY = ((Circle)(t.getSource())).getTranslateY();
                        }
                    };

            EventHandler<MouseEvent> nodeOnMouseDraggedEventHandler =
                    new EventHandler<MouseEvent>() {
                        @Override
                        public void handle(MouseEvent t) {
                            double offsetX = t.getSceneX() - orgSceneX;
                            double offsetY = t.getSceneY() - orgSceneY;
                            double newTranslateX = orgTranslateX + offsetX;
                            double newTranslateY = orgTranslateY + offsetY;

                            ((Circle)(t.getSource())).setTranslateX(newTranslateX);
                            ((Circle)(t.getSource())).setTranslateY(newTranslateY);
                        }
                    };
            EventHandler<MouseEvent> onItemClicked =
                    new EventHandler<MouseEvent>() {
                        @Override
                        public void handle(MouseEvent mouseEvent) {
                            node.toggleSelection();
                            if (nodeSelected == -1) {
                                nodeSelected = node.ID;
                                infoField.setText("Selected node ID: " + nodeSelected);
                            }
                            else
                                for (int i = 0; i < nodeList.size(); i++) {
                                    if (nodeList.get(i).ID == nodeSelected) {
                                        nodeList.get(i).toggleSelection();
                                    }
                                }
                             nodeSelected = node.ID;
                             infoField.setText("Selected node ID: " + nodeSelected);
                        }
                    };
            node.setOnMousePressed(nodeOnMousePressedEventHandler);
            node.setOnMouseDragged(nodeOnMouseDraggedEventHandler);
            node.setOnMouseClicked(onItemClicked);
            nodeList.add(node);
            mainPane.getChildren().add(node);
            id++;
        }
        catch (Exception e) {
        }
    }

    @FXML protected void deleteNode() {
        if (nodeSelected != -1) {
            for (int i = 0; i < nodeList.size(); i++) {
                if (nodeList.get(i).ID == nodeSelected) {
                    Node temp = nodeList.get(i);
                    nodeList.remove(i);
                    mainPane.getChildren().remove(temp);
                }
            }
        infoField.setText(null);
        nodeSelected = -1;
        }
    }

    @FXML protected void saveConfig() {

    }

    @FXML protected void addLink() {

    }

    @FXML protected void deleteLink() {

    }
}
