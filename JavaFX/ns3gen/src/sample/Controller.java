package sample;

import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseButton;
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
    List<Link> linkList = new ArrayList<>();
    List<Node> nodeList = new ArrayList<>();
    int nodeSelected = -1;
    Node firstNode = null;
    boolean linkMode = false;
    @FXML
    public TextField asTxtField;
    @FXML
    public Text infoField;
    @FXML
    public Pane mainPane;

    @FXML
    protected void addNewNode() {
        try {
            Node node = new Node(id, Integer.parseInt(asTxtField.getText()));
            System.out.println(asTxtField.getText());
            EventHandler<MouseEvent> nodeOnMousePressedEventHandler =
                    t -> {
                        orgSceneX = t.getSceneX();
                        orgSceneY = t.getSceneY();
                        orgTranslateX = ((Circle) (t.getSource())).getTranslateX();
                        orgTranslateY = ((Circle) (t.getSource())).getTranslateY();
                    };

            EventHandler<MouseEvent> nodeOnMouseDraggedEventHandler =
                    t -> {
                        double offsetX = t.getSceneX() - orgSceneX;
                        double offsetY = t.getSceneY() - orgSceneY;
                        double newTranslateX = orgTranslateX + offsetX;
                        double newTranslateY = orgTranslateY + offsetY;

                        ((Circle) (t.getSource())).setTranslateX(newTranslateX);
                        ((Circle) (t.getSource())).setTranslateY(newTranslateY);
                        node.updatePos(newTranslateX, newTranslateY);
                        updateLinks(node);
                    };
            EventHandler<MouseEvent> onItemClicked =
                    t -> {
                        if (linkMode == true) {
                            Node secondNode = node;
                            infoField.setText("LINK CREATED BETWEEN: " + firstNode.ID + " " + secondNode.ID);

                            Link link = new Link(firstNode, secondNode);
                            linkList.add(link);
                            //DEBUG
                            for (int i = 0; i < linkList.size(); i++) {
                                Link temp = linkList.get(i);
                                System.out.println(i + ": " + temp.firstNode.ID + " " + temp.secondNode.ID);
                            }
                            //END DEBUG
                            link.setOnMouseClicked(mouseEvent -> {
                                if (mouseEvent.getButton().equals(MouseButton.SECONDARY)) {
                                    linkList.remove(link);
                                    mainPane.getChildren().remove(link);
                                }
                            });
                            mainPane.getChildren().add(link);
                            link.toBack();
                            firstNode.toggleSelection();
                            nodeSelected = -1;
                            firstNode = null;
                            linkMode = false;
                        } else {
                            node.toggleSelection();
                            if (nodeSelected == -1) {
                                nodeSelected = node.ID;
                                infoField.setText("Selected node ID: " + nodeSelected);
                            } else
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
        } catch (Exception e) {
        }
    }

    private void updateLinks(Node node) {
        Link temp;
        for (int i = 0; i < linkList.size(); i++) {
            temp = linkList.get(i);
            if (temp.firstNode.ID == node.ID) {
                System.out.println("UPDATING FIRST NODE");
                temp.setStartX(node.xPos);
                temp.setStartY(node.yPos);
            } else if (temp.secondNode.ID == node.ID) {
                System.out.println("UPDATING SECOND NODE");
                temp.setEndX(node.xPos);
                temp.setEndY(node.yPos);
            }
        }
    }

    @FXML
    protected void deleteNode() {
        if (nodeSelected != -1) {
            for (int i = 0; i < nodeList.size(); i++) {
                if (nodeList.get(i).ID == nodeSelected) {
                    Node temp = nodeList.get(i);
                    nodeList.remove(i);
                    removeLinks(temp);
                    mainPane.getChildren().remove(temp);
                }
            }
            infoField.setText(null);
            nodeSelected = -1;
        }
    }

    @FXML
    protected void saveConfig() {

    }

    @FXML
    protected void addLink() throws InterruptedException {
        if (nodeSelected == -1) {
            infoField.setText("Select a node first");
        } else {
            infoField.setText("Select second node");
            for (int i = 0; i < nodeList.size(); i++) {
                if (nodeList.get(i).ID == nodeSelected) {
                    firstNode = nodeList.get(i);
                }
            }
            linkMode = true;
        }
    }

    private void removeLinks(Node node) {
        Link temp;
        for (int i = 0; i < linkList.size(); i++) {
            temp = linkList.get(i);
            if (temp.firstNode.ID == node.ID) {
                mainPane.getChildren().remove(temp);
                linkList.remove(temp);
                i=-1;
            }
            else if (temp.secondNode.ID == node.ID) {
                mainPane.getChildren().remove(temp);
                linkList.remove(temp);
                i=-1;
            }

        }
    }
}
