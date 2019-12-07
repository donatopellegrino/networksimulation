package sample;

import javafx.scene.paint.Color;
import javafx.scene.shape.Line;

import java.awt.*;
import java.awt.event.MouseEvent;

public class Link extends Line {
    Node firstNode;
    Node secondNode;

    Link(Node first, Node second) {
        this.firstNode = first;
        this.secondNode = second;
        this.setStartX(firstNode.xPos);
        this.setStartY(firstNode.yPos);
        this.setEndX(secondNode.xPos);
        this.setEndY(secondNode.yPos);
        this.setStrokeWidth(3);
        this.setStroke(Color.BLACK);

    }
}
