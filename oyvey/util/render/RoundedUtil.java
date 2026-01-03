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

        // Center vertex for the fan. The GuiGraphics context handles the matrix transformations.
        bufferBuilder.addVertex((float)(x + width / 2), (float)(y + height / 2), 0).setColor(r, g, b, a);

        // Draw the circumference of the rounded rectangle
        final int numSegments = 16;
        // Top-left corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 1.5 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex((float)(x + radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Top-right corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 2.0 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex((float)(x + width - radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Bottom-right corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 0.5 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex((float)(x + width - radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Bottom-left corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex((float)(x + radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Close the fan by connecting back to the start of the first corner
        double startAngle = Math.PI * 1.5;
        bufferBuilder.addVertex((float)(x + radius + Math.cos(startAngle) * radius), (float)(y + radius + Math.sin(startAngle) * radius), 0).setColor(r, g, b, a);

        // Draw using the project's custom layer system, which handles the shader state.
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
