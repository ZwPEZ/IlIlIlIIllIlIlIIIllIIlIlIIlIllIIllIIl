package me.alpha432.oyvey.util.render;

import com.mojang.blaze3d.vertex.BufferUploader;
import com.mojang.blaze3d.vertex.VertexBuffer;
import me.alpha432.oyvey.util.traits.Util;
import net.minecraft.client.renderer.RenderPipelines;

import java.util.Optional;

public class Layers implements Util {
    private static VertexBuffer.RenderType globalQuads;
    private static VertexBuffer.RenderType globalLines;

    public static VertexBuffer.RenderType getGlobalQuads() {
        if (globalQuads == null) {
            globalQuads = new VertexBuffer.RenderType(
                    "global_quads",
                    () -> Pipelines.getGlobalRenders().get(0),
                    () -> {
                        Optional.ofNullable(Pipelines.getGlobalRenders().get(0)).ifPresent(RenderPipelines.ShaderProgram::bind);
                    },
                    () -> {
                        Optional.ofNullable(Pipelines.getGlobalRenders().get(0)).ifPresent(RenderPipelines.ShaderProgram::unbind);
                    },
                    false
            );
        }
        return globalQuads;
    }

    public static VertexBuffer.RenderType getGlobalLines(double lineWidth) {
        if (globalLines == null) {
            globalLines = new VertexBuffer.RenderType(
                    "global_lines",
                    () -> Pipelines.getGlobalRenders().get(1),
                    () -> {
                        Optional.ofNullable(Pipelines.getGlobalRenders().get(1)).ifPresent(shader -> {
                            shader.bind();
                            shader.getUniform("lineWidth").set((float) lineWidth);
                        });
                    },
                    () -> {
                        Optional.ofNullable(Pipelines.getGlobalRenders().get(1)).ifPresent(RenderPipelines.ShaderProgram::unbind);
                    },
                    false
            );
        }
        return globalLines;
    }
}
