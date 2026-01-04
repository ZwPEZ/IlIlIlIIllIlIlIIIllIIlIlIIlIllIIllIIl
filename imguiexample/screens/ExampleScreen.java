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

    // Animation state variables
    private final float[] hoverAlphas = new float[Module.Category.values().length];
    private final float[] selectionFades = new float[Module.Category.values().length];
    private final float animationSpeed = 15f;
    private float topBarHeight = 0f;

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

        topBarHeight = ImGui.getCursorPosY();
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
        float contentHeight = ImGui.getWindowSizeY() - topSeparatorY - topBarHeight - 10;
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
        float bottomBarY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - topBarHeight;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        ImGui.getWindowDrawList().addLine(windowPosX, bottomBarY, windowPosX + windowWidth, bottomBarY, borderColor);
    }

    private void renderCategoryButtons() {
        float buttonHeight = 20.0f;
        float categoryButtonY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - (topBarHeight / 2) - (buttonHeight / 2);
        float buttonSpacing = 10.0f;
        float horizontalPadding = 20.0f;
        float rounding = 4.0f;
        int defaultBorderColor = ImGui.getColorU32(ImGuiCol.Border);
        float deltaTime = ImGui.getIO().getDeltaTime();

        Module.Category[] categories = Module.Category.values();

        float[] buttonWidths = new float[categories.length];
        float totalWidth = 0;
        for (int i = 0; i < categories.length; i++) {
            buttonWidths[i] = ImGui.calcTextSize(categories[i].getName()).x + horizontalPadding;
            totalWidth += buttonWidths[i];
        }
        totalWidth += (categories.length - 1) * buttonSpacing;

        float currentX = ImGui.getWindowPosX() + (ImGui.getWindowSizeX() - totalWidth) / 2.0f;

        for (int i = 0; i < categories.length; i++) {
            Module.Category category = categories[i];
            String name = category.getName();
            float buttonWidth = buttonWidths[i];
            float buttonX = currentX;
            float buttonY = categoryButtonY;

            boolean hovered = ImGui.isMouseHoveringRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
            if (hovered && ImGui.isMouseClicked(0)) {
                selectedCategory = category;
            }

            // Animate hover and selection
            float targetSelection = (category == selectedCategory) ? 1.0f : 0.0f;
            selectionFades[i] += (targetSelection - selectionFades[i]) * deltaTime * animationSpeed;

            if (hovered) {
                hoverAlphas[i] = Math.min(1.0f, hoverAlphas[i] + deltaTime * animationSpeed);
            } else {
                hoverAlphas[i] = Math.max(0.0f, hoverAlphas[i] - deltaTime * animationSpeed);
            }

            // Draw background
            int bgColor = ImGui.getColorU32(0.2f, 0.2f, 0.2f, 0.5f + (hoverAlphas[i] * 0.3f));
            ImGui.getWindowDrawList().addRectFilled(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, bgColor, rounding);

            // Interpolate colors for text and outline
            float accentR = 255f / 255f, accentG = 110f / 255f, accentB = 110f / 255f;
            int textColor = ImGui.getColorU32(ImGuiCol.Text);

            float r = (textColor >> 0 & 0xFF) / 255f;
            float g = (textColor >> 8 & 0xFF) / 255f;
            float b = (textColor >> 16 & 0xFF) / 255f;

            float textR = r + (accentR - r) * selectionFades[i];
            float textG = g + (accentG - g) * selectionFades[i];
            float textB = b + (accentB - b) * selectionFades[i];
            int animatedTextColor = ImGui.getColorU32(textR, textG, textB, 1.0f);

            int outlineColor = defaultBorderColor;
            r = (outlineColor >> 0 & 0xFF) / 255f;
            g = (outlineColor >> 8 & 0xFF) / 255f;
            b = (outlineColor >> 16 & 0xFF) / 255f;

            float outlineR = r + (accentR - r) * selectionFades[i];
            float outlineG = g + (accentG - g) * selectionFades[i];
            float outlineB = b + (accentB - b) * selectionFades[i];
            int animatedOutlineColor = ImGui.getColorU32(outlineR, outlineG, outlineB, 1.0f);

            // Draw outline
            ImGui.getWindowDrawList().addRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, animatedOutlineColor, rounding);

            // Draw text
            float textWidth = ImGui.calcTextSize(name).x;
            float textX = buttonX + (buttonWidth - textWidth) / 2.0f;
            float textY = buttonY + (buttonHeight - ImGui.getTextLineHeight()) / 2.0f;
            ImGui.getWindowDrawList().addText(textX, textY, animatedTextColor, name);

            currentX += buttonWidth + buttonSpacing;
        }
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }
}
