package sample;

import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;

public class Node extends Circle {
    int ID;
    int AS;
    boolean selected = false;

    Node(int id, int as) {
        Color color;
        this.ID = id;
        this.AS = as;
        this.setCenterX(30);
        this.setCenterY(30);
        this.setRadius(10);
        this.setStrokeWidth(3);
        switch (as) {
            case 1: color = Color.RED; break;
            case 2: color = Color.BLUE; break;
            case 3: color = Color.GREEN; break;
            case 4: color = Color.YELLOW; break;
            case 5: color = Color.BROWN; break;
            case 6: color = Color.ORANGE; break;
            case 7: color = Color.GOLD; break;
            case 8: color = Color.WHITE; break;
            case 9: color = Color.AQUA; break;
            default: color = Color.GRAY;
        }
        this.setFill(color);

    }

    public void toggleSelection() {
        if (!selected) {
            this.setStroke(Color.BLACK);
            selected = true;
        }
        else {
            this.setStroke(null);
            selected = false;
        }
    }
}
