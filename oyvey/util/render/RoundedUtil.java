package me.alpha432.oyvey.util.render;

import com.mojang.blaze3d.vertex.BufferBuilder;
import com.mojang.blaze3d.vertex.DefaultVertexFormat;
import com.mojang.blaze3d.vertex.Tesselator;
import com.mojang.blaze3d.vertex.VertexFormat;
import me.alpha432.oyvey.util.traits.Util;
import net.minecraft.client.gui.GuiGraphics;
import org.joml.Matrix4f;

import java.awt.Color;

public class RoundedUtil implements Util {
    public static void rect(GuiGraphics context, double x, double y, double width, double height, double radius, Color color) {
        Matrix4f matrix = context.pose().pose();
        float f = (float) (color.getRGB() >> 24 & 255) / 255.0F;
        float f1 = (float) (color.getRGB() >> 16 & 255) / 255.0F;
        float f2 = (float) (color.getRGB() >> 8 & 255) / 255.0F;
        float f3 = (float) (color.getRGB() & 255) / 255.0F;

        BufferBuilder bufferBuilder = Tesselator.getInstance().begin(VertexFormat.Mode.TRIANGLE_FAN, DefaultVertexFormat.POSITION_COLOR);

        // Draw the center
        bufferBuilder.addVertex(matrix, (float) (x + width / 2), (float) (y + height / 2), 0).setColor(f1, f2, f3, f);

        // Draw the four corners
        for (int i = 0; i <= 90; i += 3) {
            double angle = Math.toRadians(i);
            bufferBuilder.addVertex(matrix, (float) (x + radius + Math.sin(angle) * radius), (float) (y + radius - Math.cos(angle) * radius), 0).setColor(f1, f2, f3, f);
        }
        for (int i = 90; i <= 180; i += 3) {
            double angle = Math.toRadians(i);
            bufferBuilder.addVertex(matrix, (float) (x + width - radius + Math.sin(angle) * radius), (float) (y + radius - Math.cos(angle) * radius), 0).setColor(f1, f2, f3, f);
        }
        for (int i = 180; i <= 270; i += 3) {
            double angle = Math.toRadians(i);
            bufferBuilder.addVertex(matrix, (float) (x + width - radius + Math.sin(angle) * radius), (float) (y + height - radius - Math.cos(angle) * radius), 0).setColor(f1, f2, f3, f);
        }
        for (int i = 270; i <= 360; i += 3) {
            double angle = Math.toRadians(i);
            bufferBuilder.addVertex(matrix, (float) (x + radius + Math.sin(angle) * radius), (float) (y + height - radius - Math.cos(angle) * radius), 0).setColor(f1, f2, f3, f);
        }
        bufferBuilder.addVertex(matrix, (float) (x + radius), (float) y, 0).setColor(f1, f2, f3, f);

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
