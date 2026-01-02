package me.alpha432.oyvey.util.render;

import com.mojang.blaze3d.vertex.BufferBuilder;
import com.mojang.blaze3d.vertex.DefaultVertexFormat;
import com.mojang.blaze3d.vertex.Tesselator;
import com.mojang.blaze3d.vertex.VertexFormat;
import me.alpha432.oyvey.util.traits.Util;
import net.minecraft.client.gui.GuiGraphics;
import org.joml.Matrix3x2f;
import org.joml.Matrix4f;

import java.awt.Color;

public class RoundedUtil implements Util {
    public static void rect(GuiGraphics context, double x, double y, double width, double height, double radius, Color color) {
        // Correctly get the 3x2 matrix from the stack
        Matrix3x2f mat3x2 = context.pose().last();

        // Convert the 3x2 matrix to a 4x4 matrix suitable for the BufferBuilder
        Matrix4f matrix = new Matrix4f(
                mat3x2.m00, mat3x2.m01, 0.0f, 0.0f,
                mat3x2.m10, mat3x2.m11, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                mat3x2.m20, mat3x2.m21, 0.0f, 1.0f
        );

        float r = (float) (color.getRed()) / 255.0F;
        float g = (float) (color.getGreen()) / 255.0F;
        float b = (float) (color.getBlue()) / 255.0F;
        float a = (float) (color.getAlpha()) / 255.0F;

        BufferBuilder bufferBuilder = Tesselator.getInstance().begin(VertexFormat.Mode.TRIANGLE_FAN, DefaultVertexFormat.POSITION_COLOR);

        // Center vertex
        bufferBuilder.addVertex(matrix, (float)(x + width / 2), (float)(y + height / 2), 0).setColor(r, g, b, a);

        final int numSegments = 15;
        // Top-left corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 1.5 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex(matrix, (float)(x + radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Top-right corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 2.0 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex(matrix, (float)(x + width - radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Bottom-right corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI * 0.5 + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex(matrix, (float)(x + width - radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Bottom-left corner
        for (int i = 0; i <= numSegments; i++) {
            double angle = Math.PI + (Math.PI / 2 * i / numSegments);
            bufferBuilder.addVertex(matrix, (float)(x + radius + Math.cos(angle) * radius), (float)(y + height - radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);
        }
        // Close the fan
        double angle = Math.PI * 1.5;
        bufferBuilder.addVertex(matrix, (float)(x + radius + Math.cos(angle) * radius), (float)(y + radius + Math.sin(angle) * radius), 0).setColor(r, g, b, a);

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
