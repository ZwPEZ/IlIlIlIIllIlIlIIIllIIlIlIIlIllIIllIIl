package de.florianmichael.imguiexample.screens;

import de.florianmichael.imguiexample.imgui.RenderInterface;
import imgui.ImGui;
import imgui.flag.ImGuiStyleVar;
import imgui.ImGuiIO;
import imgui.flag.ImGuiCol;
import imgui.flag.ImGuiCond;
import imgui.flag.ImGuiWindowFlags;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;

public final class ExampleScreen extends Screen implements RenderInterface {
    public ExampleScreen() {
        super(Component.literal("mKys50oVNZFC9f07FkrwAgCEhT9fqv69TeKhN1Z8VJTDfE8jOJ"));
    }

    @Override
    public void render(ImGuiIO io) {
        float MenuWidth = 850;
        float MenuHeight = 520;

        float centerX = (ImGui.getIO().getDisplaySizeX() - MenuWidth) / 2.0f;
        float centerY = (ImGui.getIO().getDisplaySizeY() - MenuHeight) / 2.0f;

        ImGui.setNextWindowSize(MenuWidth, MenuHeight, ImGuiCond.Once);
        ImGui.setNextWindowPos(centerX, centerY, ImGuiCond.Once);
        ImGui.setNextWindowSize(MenuWidth, MenuHeight, ImGuiCond.Once);

        if (ImGui.begin("Siphfm5JfiGW3sWYyxdCxHIG8D8zLoFXRt9e5FPBh2OFSQuV6B", ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoTitleBar)) {
            float glowR = 255f / 255f;
            float glowG = 110f / 255f;
            float glowB = 110f / 255f;

            String left = "Demon ";
            String right = "Client";

            float leftWidth = ImGui.calcTextSize(left).x;
            float rightWidth = ImGui.calcTextSize(right).x;
            float totalWidth = leftWidth + rightWidth;

            ImGui.setCursorPosX((ImGui.getWindowSizeX() - totalWidth) * 0.5f);

            ImGui.text(left);
            ImGui.sameLine(0, 0);

            float time = (float) ImGui.getTime();

            float x = ImGui.getCursorScreenPosX();
            float y = ImGui.getCursorScreenPosY();

            float r1 = 255f / 255f, g1 = 110f / 255f, b1 = 110f / 255f;
            float r2 = 255f / 255f, g2 = 180f / 255f, b2 = 180f / 255f;

            for (int i = 0; i < right.length(); i++) {
                char c = right.charAt(i);
                String s = String.valueOf(c);

                float charWidth = ImGui.calcTextSize(s).x;

                float wave = (float) Math.sin(time * 3.0f + i * 0.6f) * 0.5f + 0.5f;

                float r = r1 + (r2 - r1) * wave;
                float g = g1 + (g2 - g1) * wave;
                float b = b1 + (b2 - b1) * wave;

                int color = ImGui.getColorU32(r, g, b, 1.0f);
                ImGui.getWindowDrawList().addText(x, y, color, s);

                x += charWidth;
            }

            ImGui.dummy(rightWidth, ImGui.getTextLineHeight());
            ImGui.dummy(0, 2);

            float windowPosX = ImGui.getWindowPosX();
            float windowPosY = ImGui.getCursorScreenPosY();
            float windowWidth = ImGui.getWindowSizeX();

            int glowLineColor = ImGui.getColorU32(glowR, glowG, glowB, 1.0f);
            ImGui.getWindowDrawList().addLine(windowPosX, windowPosY, windowPosX + windowWidth, windowPosY, glowLineColor);

            float glowTopY = windowPosY - 27;
            float glowBottomY = windowPosY;

            int colorTop = ImGui.getColorU32(glowR, glowG, glowB, 0.0f);
            int colorBottom = ImGui.getColorU32(glowR, glowG, glowB, 0.13f);

            ImGui.getWindowDrawList().addRectFilledMultiColor(windowPosX, glowTopY, windowPosX + windowWidth, glowBottomY, colorTop, colorTop, colorBottom, colorBottom);

            float bottomPadding = 50.0f;
            float bottomLineY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - bottomPadding;

            int borderColor = ImGui.getColorU32(ImGuiCol.Border);

            ImGui.getWindowDrawList().addLine(windowPosX, bottomLineY, windowPosX + windowWidth, bottomLineY, borderColor);

            ImGui.end();
        }
    }

    @Override
    public boolean isPauseScreen() {
        return false; // Only relevant in singleplayer
    }
}
