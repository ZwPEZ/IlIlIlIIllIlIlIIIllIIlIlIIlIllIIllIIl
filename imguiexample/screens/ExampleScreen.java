package de.florianmichael.imguiexample.screens;

import de.florianmichael.imguiexample.features.modules.Module;
import de.florianmichael.imguiexample.imgui.RenderInterface;
import de.florianmichael.imguiexample.manager.ModuleManager;
import imgui.ImGui;
import imgui.ImGuiIO;
import imgui.flag.ImGuiCol;
import imgui.flag.ImGuiCond;
import imgui.flag.ImGuiWindowFlags;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;

import java.util.List;

public final class ExampleScreen extends Screen implements RenderInterface {
    private final ModuleManager moduleManager;
    private Module.Category selectedCategory;

    public ExampleScreen() {
        super(Component.literal("Example IMGUI Screen"));
        this.moduleManager = new ModuleManager();
        if (Module.Category.values().length > 0) {
            this.selectedCategory = Module.Category.values()[0];
        }
    }

    @Override
    public void render(ImGuiIO io) {
        float menuWidth = 850;
        float menuHeight = 520;

        float centerX = (ImGui.getIO().getDisplaySizeX() - menuWidth) / 2.0f;
        float centerY = (ImGui.getIO().getDisplaySizeY() - menuHeight) / 2.0f;

        ImGui.setNextWindowSize(menuWidth, menuHeight, ImGuiCond.Once);
        ImGui.setNextWindowPos(centerX, centerY, ImGuiCond.Once);

        if (ImGui.begin("Demon Client", ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoTitleBar)) {
            renderTitle();
            renderTopSeparator();
            renderModuleSections();
            renderBottomSeparator();
            renderCategoryButtons();
            ImGui.end();
        }
    }

    private void renderTitle() {
        String left = "Demon ";
        String right = "Client";

        float leftWidth = ImGui.calcTextSize(left).x;
        float rightWidth = ImGui.calcTextSize(right).x;
        float totalWidth = leftWidth + rightWidth;

        ImGui.setCursorPosX((ImGui.getWindowSizeX() - totalWidth) * 0.5f);

        ImGui.text(left);
        ImGui.sameLine(0, 0);

        // Animated "Client" text
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

        // Dummy to take up space
        ImGui.dummy(rightWidth, ImGui.getTextLineHeight());
        ImGui.dummy(0, 2);
    }

    private void renderTopSeparator() {
        float windowPosX = ImGui.getWindowPosX();
        float windowWidth = ImGui.getWindowSizeX();
        float separatorY = ImGui.getCursorScreenPosY();

        // Glow colors
        float glowR = 255f / 255f;
        float glowG = 110f / 255f;
        float glowB = 110f / 255f;

        // Separator line
        int lineColor = ImGui.getColorU32(glowR, glowG, glowB, 1.0f);
        ImGui.getWindowDrawList().addLine(windowPosX, separatorY, windowPosX + windowWidth, separatorY, lineColor);

        // Glow effect
        float glowTopY = separatorY - 27;
        int colorTop = ImGui.getColorU32(glowR, glowG, glowB, 0.0f);
        int colorBottom = ImGui.getColorU32(glowR, glowG, glowB, 0.13f);
        ImGui.getWindowDrawList().addRectFilledMultiColor(windowPosX, glowTopY, windowPosX + windowWidth, separatorY, colorTop, colorTop, colorBottom, colorBottom);
    }

    private void renderModuleSections() {
        float topSeparatorY = ImGui.getCursorPosY();
        float bottomSeparatorY = ImGui.getWindowSizeY() - 50.0f;
        float contentHeight = bottomSeparatorY - topSeparatorY - 20;
        float spacing = 10.0f;
        float totalSpacing = spacing * 4;
        float sectionWidth = (ImGui.getWindowSizeX() - totalSpacing) / 3;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        float rounding = 4.0f;

        List<Module> modules = moduleManager.getModulesByCategory(selectedCategory);
        int moduleCount = modules.size();
        int modulesPerSection = (int) Math.ceil(moduleCount / 3.0);

        float windowPosX = ImGui.getWindowPosX();
        float windowPosY = ImGui.getWindowPosY();

        // Left Section
        float leftX = windowPosX + spacing;
        float sectionY = windowPosY + topSeparatorY + spacing;
        ImGui.getWindowDrawList().addRect(leftX, sectionY, leftX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(spacing * 1.5f, topSeparatorY + spacing * 1.5f);
        ImGui.beginChild("LeftSection", sectionWidth - spacing, contentHeight - spacing, false);
        for (int i = 0; i < modulesPerSection && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, spacing);

        // Middle Section
        float middleX = leftX + sectionWidth + spacing;
        ImGui.getWindowDrawList().addRect(middleX, sectionY, middleX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(spacing * 2.5f + sectionWidth, topSeparatorY + spacing * 1.5f);
        ImGui.beginChild("MiddleSection", sectionWidth - spacing, contentHeight - spacing, false);
        for (int i = modulesPerSection; i < modulesPerSection * 2 && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, spacing);

        // Right Section
        float rightX = middleX + sectionWidth + spacing;
        ImGui.getWindowDrawList().addRect(rightX, sectionY, rightX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(spacing * 3.5f + sectionWidth * 2, topSeparatorY + spacing * 1.5f);
        ImGui.beginChild("RightSection", sectionWidth - spacing, contentHeight - spacing, false);
        for (int i = modulesPerSection * 2; i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();
    }

    private void renderBottomSeparator() {
        float windowPosX = ImGui.getWindowPosX();
        float windowWidth = ImGui.getWindowSizeX();
        float bottomPadding = 50.0f;
        float bottomLineY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - bottomPadding;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        ImGui.getWindowDrawList().addLine(windowPosX, bottomLineY, windowPosX + windowWidth, bottomLineY, borderColor);
    }

    private void renderCategoryButtons() {
        float bottomPadding = 50.0f;
        float categoryButtonY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - bottomPadding + 15;
        float buttonHeight = 20.0f;
        float buttonSpacing = 10.0f;
        float horizontalPadding = 20.0f;

        Module.Category[] categories = Module.Category.values();
        float totalWidth = 0;
        for (Module.Category category : categories) {
            totalWidth += ImGui.calcTextSize(category.getName()).x + horizontalPadding;
        }
        totalWidth += (categories.length - 1) * buttonSpacing;

        float currentX = ImGui.getWindowPosX() + (ImGui.getWindowSizeX() - totalWidth) / 2.0f;

        for (Module.Category category : categories) {
            String name = category.getName();
            float buttonWidth = ImGui.calcTextSize(name).x + horizontalPadding;
            float buttonX = currentX;
            float buttonY = categoryButtonY;

            boolean hovered = ImGui.isMouseHoveringRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
            boolean clicked = hovered && ImGui.isMouseClicked(0);

            int color;
            if (category == selectedCategory) {
                float time = (float) ImGui.getTime();
                float alpha = (float) (Math.sin(time * 4.0f) * 0.5f + 0.5f) * 0.3f + 0.2f;
                color = ImGui.getColorU32(0.9f, 0.2f, 0.3f, alpha);
                ImGui.getWindowDrawList().addRectFilled(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, color, 4.0f);
            }

            color = hovered ? ImGui.getColorU32(0.4f, 0.4f, 0.4f, 0.5f) : ImGui.getColorU32(0.2f, 0.2f, 0.2f, 0.5f);
            ImGui.getWindowDrawList().addRectFilled(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, color, 4.0f);

            float textWidth = ImGui.calcTextSize(name).x;
            float textX = buttonX + (buttonWidth - textWidth) / 2.0f;
            float textY = buttonY + (buttonHeight - ImGui.getTextLineHeight()) / 2.0f;
            ImGui.getWindowDrawList().addText(textX, textY, ImGui.getColorU32(ImGuiCol.Text), name);

            if (clicked) {
                selectedCategory = category;
            }

            currentX += buttonWidth + buttonSpacing;
        }
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }
}
