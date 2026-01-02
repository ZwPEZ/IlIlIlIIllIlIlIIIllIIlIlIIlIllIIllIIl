package me.alpha432.oyvey.util.render;

import com.mojang.blaze3d.vertex.BufferBuilder;
import com.mojang.blaze3d.vertex.DefaultVertexFormat;
import com.mojang.blaze3d.vertex.Tesselator;
import com.mojang.blaze3d.vertex.VertexFormat;
import me.alpha432.oyvey.util.traits.Util;
import net.minecraft.client.gui.GuiGraphics;

import java.awt.Color;

public class RoundedUtil implements Util {
    public static void rect(GuiGraphics context, double x, double y, double width, double height, double radius, Color color) {
        float r = (float) (color.getRed()) / 255.0F;
        float g = (float) (color.getGreen()) / 255.0F;
        float b = (float) (color.getBlue()) / 255.0F;
        float a = (float) (color.getAlpha()) / 255.0F;

        BufferBuilder bufferBuilder = Tesselator.getInstance().begin(VertexFormat.Mode.TRIANGLE_FAN, DefaultVertexFormat.POSITION_COLOR);

        // Center vertex of the fan
        bufferBuilder.addVertex((float)(x + width / 2), (float)(y + height / 2), 0).setColor(r, g, b, a);

        // Create the fan sections for each corner and the straight edges
        // Top-left corner
        for (int i = 0; i <= 90; i += 3) {
            double angle = Math.toRadians(180 + i);
            bufferBuilder.addVertex((float)(x + radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }

        // Top-right corner
        for (int i = 0; i <= 90; i += 3) {
            double angle = Math.toRadians(270 + i);
            bufferBuilder.addVertex((float)(x + width - radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }

        // Bottom-right corner
        for (int i = 0; i <= 90; i += 3) {
            double angle = Math.toRadians(i);
            bufferBuilder.addVertex((float)(x + width - radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }

        // Bottom-left corner
        for (int i = 0; i <= 90; i += 3) {
            double angle = Math.toRadians(90 + i);
            bufferBuilder.addVertex((float)(x + radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }

        // Close the fan by connecting back to the start of the first corner
        double startAngle = Math.toRadians(180);
        bufferBuilder.addVertex((float)(x + radius + Math.cos(startAngle) * radius), (float)(y + radius + Math.sin(startAngle) * radius), 0).setColor(r, g, b, a);


        Layers.getGlobalQuads().draw(bufferBuilder.buildOrThrow());
    }

    public static void rect(GuiGraphics context, double x, double y, double width, double height, double radius, int color) {
        rect(context, x, y, width, height, radius, new Color(color));
    }

    public static void rect(GuiGraphics context, double x, double y, double width, double height, boolean filled, double radius, Color color) {
        if (filled) {
            rect(context, x, y, width, height, radius, color);
        }
    }
}
