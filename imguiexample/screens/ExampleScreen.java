package de.florianmichael.imguiexample.screens;

import de.florianmichael.imguiexample.ExampleMod;
import de.florianmichael.imguiexample.features.modules.Module;
import de.florianmichael.imguiexample.features.modules.Section;
import de.florianmichael.imguiexample.imgui.RenderInterface;
import imgui.ImGui;
import imgui.ImGuiIO;
import imgui.flag.ImGuiCol;
import imgui.flag.ImGuiCond;
import imgui.flag.ImGuiWindowFlags;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;
import org.lwjgl.glfw.GLFW;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public final class ExampleScreen extends Screen implements RenderInterface {
    private static Module.Category selectedCategory;
    private Module bindingModule = null;

    // Constants
    private static final float ACCENT_R = 255f / 255f;
    private static final float ACCENT_G = 110f / 255f;
    private static final float ACCENT_B = 110f / 255f;
    private static final float SPACING = 10.0f;
    private static final float BAR_HEIGHT = 40.0f;

    // Animation state variables
    private final float[] hoverAlphas = new float[Module.Category.values().length];
    private final float[] selectionFades = new float[Module.Category.values().length];
    private final Map<Module, Float> moduleSelectionFades = new HashMap<>();
    private final Map<String, Float> buttonHoverAlphas = new HashMap<>();
    private final Map<String, Float> toggleAnimationFades = new HashMap<>();
    private final float animationSpeed = 15f;

    public ExampleScreen() {
        super(Component.literal("Example IMGUI Screen"));
        if (selectedCategory == null && Module.Category.values().length > 0) {
            selectedCategory = Module.Category.values()[0];
        }

        // Initialize animation states to prevent color flash on open
        for (int i = 0; i < Module.Category.values().length; i++) {
            if (Module.Category.values()[i] == selectedCategory) {
                selectionFades[i] = 1.0f;
            }
        }
        for (Module module : ExampleMod.moduleManager.getModules()) {
            if (module.isEnabled()) {
                moduleSelectionFades.put(module, 1.0f);
            }
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

        float titleY = (BAR_HEIGHT - ImGui.getTextLineHeight()) / 2;
        ImGui.setCursorPosY(titleY);
        ImGui.setCursorPosX((ImGui.getWindowSizeX() - totalWidth) / 2.0f);

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
    }

    private void renderTopSeparator() {
        ImGui.setCursorPosY(BAR_HEIGHT);
        float windowPosX = ImGui.getWindowPosX();
        float windowWidth = ImGui.getWindowSizeX();
        float separatorY = ImGui.getCursorScreenPosY();

        // Separator line
        int lineColor = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 1.0f);
        ImGui.getWindowDrawList().addLine(windowPosX, separatorY, windowPosX + windowWidth, separatorY, lineColor);

        // Glow effect
        float glowTopY = separatorY - 27;
        int colorTop = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 0.0f);
        int colorBottom = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 0.13f);
        ImGui.getWindowDrawList().addRectFilledMultiColor(windowPosX, glowTopY, windowPosX + windowWidth, separatorY, colorTop, colorTop, colorBottom, colorBottom);
    }

    private void renderModuleSections() {
        float topSeparatorY = BAR_HEIGHT;
        float contentHeight = ImGui.getWindowSizeY() - (BAR_HEIGHT * 2) - (SPACING * 2);
        float totalSpacing = SPACING * 4;
        float sectionWidth = (ImGui.getWindowSizeX() - totalSpacing) / 3;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        float rounding = 4.0f;

        List<Module> modules = ExampleMod.moduleManager.getModulesByCategory(selectedCategory);

        List<Module> leftModules = modules.stream().filter(m -> m.getSection() == Section.LEFT).collect(Collectors.toList());
        List<Module> middleModules = modules.stream().filter(m -> m.getSection() == Section.MIDDLE).collect(Collectors.toList());
        List<Module> rightModules = modules.stream().filter(m -> m.getSection() == Section.RIGHT).collect(Collectors.toList());

        float windowPosX = ImGui.getWindowPosX();
        float windowPosY = ImGui.getWindowPosY();

        // Left Section
        float leftX = windowPosX + SPACING;
        float sectionY = windowPosY + topSeparatorY + SPACING;
        ImGui.getWindowDrawList().addRect(leftX, sectionY, leftX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(SPACING, topSeparatorY + SPACING);
        ImGui.beginChild("LeftSection", sectionWidth, contentHeight, false);
        renderModules(leftModules);
        ImGui.endChild();

        ImGui.sameLine(0, SPACING);

        // Middle Section
        float middleX = leftX + sectionWidth + SPACING;
        ImGui.getWindowDrawList().addRect(middleX, sectionY, middleX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(SPACING * 2 + sectionWidth, topSeparatorY + SPACING);
        ImGui.beginChild("MiddleSection", sectionWidth, contentHeight, false);
        renderModules(middleModules);
        ImGui.endChild();

        ImGui.sameLine(0, SPACING);

        // Right Section
        float rightX = middleX + sectionWidth + SPACING;
        ImGui.getWindowDrawList().addRect(rightX, sectionY, rightX + sectionWidth, sectionY + contentHeight, borderColor, rounding);
        ImGui.setCursorPos(SPACING * 3 + sectionWidth * 2, topSeparatorY + SPACING);
        ImGui.beginChild("RightSection", sectionWidth, contentHeight, false);
        renderModules(rightModules);
        ImGui.endChild();
    }

    private void renderModules(List<Module> modules) {
        float buttonHeight = 25f;
        float width = ImGui.getWindowWidth() - 20;

        ImGui.setCursorPosY(ImGui.getCursorPosY() + 10);

        for (Module module : modules) {
            ImGui.pushID(module.getName());
            float targetSelection = module.isEnabled() ? 1.0f : 0.0f;
            moduleSelectionFades.put(module, moduleSelectionFades.getOrDefault(module, 0f) + (targetSelection - moduleSelectionFades.getOrDefault(module, 0f)) * ImGui.getIO().getDeltaTime() * animationSpeed);

            // Interpolate text color
            int textColor = ImGui.getColorU32(ImGuiCol.Text);

            float r = (textColor >> 0 & 0xFF) / 255f;
            float g = (textColor >> 8 & 0xFF) / 255f;
            float b = (textColor >> 16 & 0xFF) / 255f;

            float textR = r + (ACCENT_R - r) * moduleSelectionFades.get(module);
            float textG = g + (ACCENT_G - g) * moduleSelectionFades.get(module);
            float textB = b + (ACCENT_B - b) * moduleSelectionFades.get(module);
            int animatedTextColor = ImGui.getColorU32(textR, textG, textB, 1.0f);

            int defaultBorderColor = ImGui.getColorU32(ImGuiCol.Border);
            r = (defaultBorderColor >> 0 & 0xFF) / 255f;
            g = (defaultBorderColor >> 8 & 0xFF) / 255f;
            b = (defaultBorderColor >> 16 & 0xFF) / 255f;

            float borderR = r + (ACCENT_R - r) * moduleSelectionFades.get(module);
            float borderG = g + (ACCENT_G - g) * moduleSelectionFades.get(module);
            float borderB = b + (ACCENT_B - b) * moduleSelectionFades.get(module);
            int animatedBorderColor = ImGui.getColorU32(borderR, borderG, borderB, 1.0f);

            if (customButton(module.getName(), width, buttonHeight, animatedTextColor, animatedBorderColor)) {
                module.toggle();
            }

            if (ImGui.isItemHovered()) {
                ImGui.beginTooltip();
                ImGui.text(module.getDescription());
                ImGui.endTooltip();
            }

            if (ImGui.isItemClicked(1)) {
                ImGui.openPopup(module.getName() + " Settings");
            }

            ImGui.setNextWindowPos(ImGui.getIO().getDisplaySizeX() / 2, ImGui.getIO().getDisplaySizeY() / 2, ImGuiCond.Always, 0.5f, 0.5f);
            ImGui.setNextWindowSize(300, 150, ImGuiCond.Once);

            if (ImGui.beginPopupModal(module.getName() + " Settings", ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoScrollbar)) {
                String title = module.getName() + " Settings";
                float titleWidth = ImGui.calcTextSize(title).x;
                ImGui.setCursorPosX((ImGui.getWindowSizeX() - titleWidth) / 2.0f);
                ImGui.text(title);

                // Top Separator
                float modalPosX = ImGui.getWindowPosX();
                float modalWidth = ImGui.getWindowSizeX();
                ImGui.setCursorPosY(ImGui.getCursorPosY() + 4);
                float separatorY = ImGui.getCursorScreenPosY();
                int lineColor = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 1.0f);
                ImGui.getWindowDrawList().addLine(modalPosX, separatorY, modalPosX + modalWidth, separatorY, lineColor);
                float glowTopY = separatorY - 27;
                int colorTop = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 0.0f);
                int colorBottom = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 0.13f);
                ImGui.getWindowDrawList().addRectFilledMultiColor(modalPosX, glowTopY, modalPosX + modalWidth, separatorY, colorTop, colorTop, colorBottom, colorBottom);

                ImGui.setCursorPosY(ImGui.getCursorPosY() + 10);
                if (customToggleSwitch("Hidden", module.isHidden(), 100, 20)) {
                    module.setHidden(!module.isHidden());
                }

                ImGui.dummy(0, 5);

                String bindText = bindingModule == module ? "Press a key..." : "Keybind: " + (module.getBind() == -1 ? "None" : GLFW.glfwGetKeyName(module.getBind(), 0));
                if (customButton(bindText, 120, 20, ImGui.getColorU32(ImGuiCol.Text))) {
                    bindingModule = module;
                }

                // Bottom Separator
                ImGui.setCursorPosY(ImGui.getWindowSizeY() - 30);
                float bottomSeparatorY = ImGui.getCursorScreenPosY();
                ImGui.getWindowDrawList().addLine(modalPosX, bottomSeparatorY, modalPosX + modalWidth, bottomSeparatorY, ImGui.getColorU32(ImGuiCol.Border));

                ImGui.setCursorPosY(ImGui.getWindowSizeY() - 25);
                if (customButton("X", 20, 20, ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 1.0f))) {
                    ImGui.closeCurrentPopup();
                }
                ImGui.endPopup();
            }
            ImGui.popID();
            ImGui.dummy(0, 5);
        }
    }

    private boolean customToggleButton(String text, boolean toggled, float width, float height) {
        float posX = (ImGui.getWindowWidth() - width) / 2;
        ImGui.setCursorPosX(posX);

        float screenPosX = ImGui.getCursorScreenPosX();
        float screenPosY = ImGui.getCursorScreenPosY();

        boolean hovered = ImGui.isMouseHoveringRect(screenPosX, screenPosY, screenPosX + width, screenPosY + height);
        boolean clicked = hovered && ImGui.isMouseClicked(0);

        float hoverAlpha = buttonHoverAlphas.getOrDefault(text, 0f);
        if (hovered) {
            hoverAlpha = Math.min(1.0f, hoverAlpha + ImGui.getIO().getDeltaTime() * animationSpeed);
        } else {
            hoverAlpha = Math.max(0.0f, hoverAlpha - ImGui.getIO().getDeltaTime() * animationSpeed);
        }
        buttonHoverAlphas.put(text, hoverAlpha);

        int bgColor = ImGui.getColorU32(0.15f, 0.15f, 0.15f, 0.5f + (hoverAlpha * 0.3f));
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);

        ImGui.getWindowDrawList().addRectFilled(screenPosX, screenPosY, screenPosX + width, screenPosY + height, bgColor, 4.0f);
        ImGui.getWindowDrawList().addRect(screenPosX, screenPosY, screenPosX + width, screenPosY + height, borderColor, 4.0f);

        int textColor;
        if (toggled) {
            textColor = ImGui.getColorU32(ACCENT_R, ACCENT_G, ACCENT_B, 1.0f);
        } else {
            textColor = ImGui.getColorU32(ImGuiCol.Text);
        }

        float textWidth = ImGui.calcTextSize(text).x;
        float textX = screenPosX + (width - textWidth) / 2;
        float textY = screenPosY + (height - ImGui.getTextLineHeight()) / 2;
        ImGui.getWindowDrawList().addText(textX, textY, textColor, text);

        ImGui.dummy(width, height);

        return clicked;
    }

    private boolean customButton(String text, float width, float height, int textColor) {
        return customButton(text, width, height, textColor, ImGui.getColorU32(ImGuiCol.Border));
    }

    private boolean customButton(String text, float width, float height, int textColor, int borderColor) {
        float posX = (ImGui.getWindowWidth() - width) / 2;
        ImGui.setCursorPosX(posX);

        float screenPosX = ImGui.getCursorScreenPosX();
        float screenPosY = ImGui.getCursorScreenPosY();

        boolean hovered = ImGui.isMouseHoveringRect(screenPosX, screenPosY, screenPosX + width, screenPosY + height);
        boolean clicked = hovered && ImGui.isMouseClicked(0);

        float hoverAlpha = buttonHoverAlphas.getOrDefault(text, 0f);
        if (hovered) {
            hoverAlpha = Math.min(1.0f, hoverAlpha + ImGui.getIO().getDeltaTime() * animationSpeed);
        } else {
            hoverAlpha = Math.max(0.0f, hoverAlpha - ImGui.getIO().getDeltaTime() * animationSpeed);
        }
        buttonHoverAlphas.put(text, hoverAlpha);

        int bgColor = ImGui.getColorU32(0.15f, 0.15f, 0.15f, 0.5f + (hoverAlpha * 0.3f));

        ImGui.getWindowDrawList().addRectFilled(screenPosX, screenPosY, screenPosX + width, screenPosY + height, bgColor, 4.0f);
        ImGui.getWindowDrawList().addRect(screenPosX, screenPosY, screenPosX + width, screenPosY + height, borderColor, 4.0f);

        float textWidth = ImGui.calcTextSize(text).x;
        float textX = screenPosX + (width - textWidth) / 2;
        float textY = screenPosY + (height - ImGui.getTextLineHeight()) / 2;
        ImGui.getWindowDrawList().addText(textX, textY, textColor, text);

        ImGui.dummy(width, height);

        return clicked;
    }

    private void renderBottomSeparator() {
        float windowPosX = ImGui.getWindowPosX();
        float windowWidth = ImGui.getWindowSizeX();
        float bottomBarY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - BAR_HEIGHT;
        int borderColor = ImGui.getColorU32(ImGuiCol.Border);
        ImGui.getWindowDrawList().addLine(windowPosX, bottomBarY, windowPosX + windowWidth, bottomBarY, borderColor);
    }

    private void renderCategoryButtons() {
        float buttonHeight = 20.0f;
        float categoryButtonY = ImGui.getWindowPosY() + ImGui.getWindowSizeY() - (BAR_HEIGHT / 2) - (buttonHeight / 2);
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
            int bgColor = ImGui.getColorU32(0.15f, 0.15f, 0.15f, 0.5f + (hoverAlphas[i] * 0.3f));
            ImGui.getWindowDrawList().addRectFilled(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, bgColor, rounding);

            // Interpolate colors for text and outline
            int textColor = ImGui.getColorU32(ImGuiCol.Text);

            float r = (textColor >> 0 & 0xFF) / 255f;
            float g = (textColor >> 8 & 0xFF) / 255f;
            float b = (textColor >> 16 & 0xFF) / 255f;

            float textR = r + (ACCENT_R - r) * selectionFades[i];
            float textG = g + (ACCENT_G - g) * selectionFades[i];
            float textB = b + (ACCENT_B - b) * selectionFades[i];
            int animatedTextColor = ImGui.getColorU32(textR, textG, textB, 1.0f);

            int outlineColor = defaultBorderColor;
            r = (outlineColor >> 0 & 0xFF) / 255f;
            g = (outlineColor >> 8 & 0xFF) / 255f;
            b = (outlineColor >> 16 & 0xFF) / 255f;

            float outlineR = r + (ACCENT_R - r) * selectionFades[i];
            float outlineG = g + (ACCENT_G - g) * selectionFades[i];
            float outlineB = b + (ACCENT_B - b) * selectionFades[i];
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
    public boolean keyPressed(int keyCode, int scanCode, int modifiers) {
        if (bindingModule != null) {
            bindingModule.setBind(keyCode == GLFW.GLFW_KEY_ESCAPE ? -1 : keyCode);
            bindingModule = null;
            return true;
        }
        return super.keyPressed(keyCode, scanCode, modifiers);
    }

    private boolean customToggleSwitch(String text, boolean toggled, float width, float height) {
        float switchWidth = height * 1.75f;
        float textWidth = ImGui.calcTextSize(text).x;
        float spacing = 5f;
        float totalContentWidth = switchWidth + spacing + textWidth;

        float containerWidth = ImGui.getWindowWidth();
        ImGui.setCursorPosX((containerWidth - totalContentWidth) / 2.0f);

        float screenPosX = ImGui.getCursorScreenPosX();
        float screenPosY = ImGui.getCursorScreenPosY();

        boolean hovered = ImGui.isMouseHoveringRect(screenPosX, screenPosY, screenPosX + totalContentWidth, screenPosY + height);
        boolean clicked = hovered && ImGui.isMouseClicked(0);

        float targetFade = toggled ? 1.0f : 0.0f;
        float currentFade = toggleAnimationFades.getOrDefault(text, targetFade);
        currentFade += (targetFade - currentFade) * ImGui.getIO().getDeltaTime() * animationSpeed;
        toggleAnimationFades.put(text, currentFade);

        float trackHeight = height * 0.6f;
        float trackY = screenPosY + (height - trackHeight) / 2;
        float trackRounding = trackHeight / 2.0f;

        float bgR1 = 0.15f, bgG1 = 0.15f, bgB1 = 0.15f;
        float bgR2 = ACCENT_R, bgG2 = ACCENT_G, bgB2 = ACCENT_B;

        float trackR = bgR1 + (bgR2 - bgR1) * currentFade;
        float trackG = bgG1 + (bgG2 - bgG1) * currentFade;
        float trackB = bgB1 + (bgB2 - bgB1) * currentFade;
        int trackColor = ImGui.getColorU32(trackR, trackG, trackB, 1.0f);

        ImGui.getWindowDrawList().addRectFilled(screenPosX, trackY, screenPosX + switchWidth, trackY + trackHeight, trackColor, trackRounding);

        float knobRadius = (height / 2.0f) * 0.8f;
        float knobMinX = screenPosX + knobRadius + (height - knobRadius * 2) / 2;
        float knobMaxX = screenPosX + switchWidth - knobRadius - (height - knobRadius * 2) / 2;
        float knobX = knobMinX + (knobMaxX - knobMinX) * currentFade;
        float knobY = screenPosY + height / 2.0f;

        int knobColor = ImGui.getColorU32(0.9f, 0.9f, 0.9f, 1.0f);
        ImGui.getWindowDrawList().addCircleFilled(knobX, knobY, knobRadius, knobColor, 16);

        float textY = screenPosY + (height - ImGui.getTextLineHeight()) / 2.0f;
        float textX = screenPosX + switchWidth + spacing;
        ImGui.getWindowDrawList().addText(textX, textY, ImGui.getColorU32(ImGuiCol.Text), text);

        ImGui.dummy(totalContentWidth, height);

        return clicked;
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }
}
