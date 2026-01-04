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
        float contentHeight = bottomSeparatorY - topSeparatorY - 10;
        float sectionWidth = (ImGui.getWindowSizeX() - 40) / 3;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        float rounding = 4.0f;

        List<Module> modules = moduleManager.getModulesByCategory(selectedCategory);
        int moduleCount = modules.size();
        int modulesPerSection = (int) Math.ceil(moduleCount / 3.0);

        // Left Section
        float leftX = ImGui.getWindowPosX() + 10;
        float leftY = ImGui.getWindowPosY() + topSeparatorY + 5;
        ImGui.getWindowDrawList().addRect(leftX, leftY, leftX + sectionWidth, leftY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(15, topSeparatorY + 10);
        ImGui.beginChild("LeftSection", sectionWidth - 10, contentHeight - 10, false);
        for (int i = 0; i < modulesPerSection && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, 20);

        // Middle Section
        float middleX = ImGui.getWindowPosX() + 20 + sectionWidth;
        float middleY = ImGui.getWindowPosY() + topSeparatorY + 5;
        ImGui.getWindowDrawList().addRect(middleX, middleY, middleX + sectionWidth, middleY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(25 + sectionWidth, topSeparatorY + 10);
        ImGui.beginChild("MiddleSection", sectionWidth - 10, contentHeight - 10, false);
        for (int i = modulesPerSection; i < modulesPerSection * 2 && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, 20);

        // Right Section
        float rightX = ImGui.getWindowPosX() + 30 + sectionWidth * 2;
        float rightY = ImGui.getWindowPosY() + topSeparatorY + 5;
        ImGui.getWindowDrawList().addRect(rightX, rightY, rightX + sectionWidth, rightY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(35 + sectionWidth * 2, topSeparatorY + 10);
        ImGui.beginChild("RightSection", sectionWidth - 10, contentHeight - 10, false);
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
        float categoryButtonY = ImGui.getWindowSizeY() - bottomPadding + 15;

        Module.Category[] categories = Module.Category.values();
        float totalButtonWidth = 0;
        for (Module.Category category : categories) {
            totalButtonWidth += ImGui.calcTextSize(category.getName()).x + 20;
        }
        totalButtonWidth += (categories.length - 1) * 10;

        float currentX = (ImGui.getWindowSizeX() - totalButtonWidth) / 2.0f;

        ImGui.setCursorPosY(categoryButtonY);

        for (int i = 0; i < categories.length; i++) {
            Module.Category category = categories[i];

            if (i == 0) {
                ImGui.setCursorPosX(currentX);
            } else {
                ImGui.sameLine(0, 10);
            }

            if (category == selectedCategory) {
                ImGui.pushStyleColor(ImGuiCol.Button, ImGui.getColorU32(0.3f, 0.3f, 0.7f, 1.0f));
                ImGui.pushStyleColor(ImGuiCol.ButtonHovered, ImGui.getColorU32(0.4f, 0.4f, 0.8f, 1.0f));
                ImGui.pushStyleColor(ImGuiCol.ButtonActive, ImGui.getColorU32(0.5f, 0.5f, 0.9f, 1.0f));
            }

            if (ImGui.button(category.getName())) {
                selectedCategory = category;
            }

            if (category == selectedCategory) {
                ImGui.popStyleColor(3);
            }
        }
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }
}
