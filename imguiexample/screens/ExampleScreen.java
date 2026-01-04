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
        float totalWidth = ImGui.calcTextSize(left).x + ImGui.calcTextSize(right).x;
        ImGui.setCursorPosX((ImGui.getWindowSizeX() - totalWidth) * 0.5f);
        ImGui.text(left);
        ImGui.sameLine(0, 0);
        ImGui.text(right);
        ImGui.dummy(0, 2);
    }

    private void renderTopSeparator() {
        float windowPosX = ImGui.getWindowPosX();
        float windowPosY = ImGui.getCursorScreenPosY();
        float windowWidth = ImGui.getWindowSizeX();
        int lineColor = ImGui.getColorU32(ImGuiCol.Border);
        ImGui.getWindowDrawList().addLine(windowPosX, windowPosY, windowPosX + windowWidth, windowPosY, lineColor);
    }

    private void renderModuleSections() {
        float topSeparatorY = ImGui.getCursorPosY();
        float bottomSeparatorY = ImGui.getWindowSizeY() - 50.0f;
        float contentHeight = bottomSeparatorY - topSeparatorY - 10;

        List<Module> modules = moduleManager.getModulesByCategory(selectedCategory);
        int moduleCount = modules.size();
        int modulesPerSection = (int) Math.ceil(moduleCount / 3.0);

        float sectionWidth = (ImGui.getWindowSizeX() - 40) / 3;

        ImGui.setCursorPos(10, topSeparatorY + 5);
        ImGui.beginChild("LeftSection", sectionWidth, contentHeight, false);
        for (int i = 0; i < modulesPerSection && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, 10);

        ImGui.beginChild("MiddleSection", sectionWidth, contentHeight, false);
        for (int i = modulesPerSection; i < modulesPerSection * 2 && i < moduleCount; i++) {
            Module module = modules.get(i);
            if (ImGui.checkbox(module.getName(), module.isEnabled())) {
                module.toggle();
            }
        }
        ImGui.endChild();

        ImGui.sameLine(0, 10);

        ImGui.beginChild("RightSection", sectionWidth, contentHeight, false);
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
