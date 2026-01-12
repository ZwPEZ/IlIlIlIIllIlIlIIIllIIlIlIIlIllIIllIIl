package me.alpha432.oyvey.features.gui;

import me.alpha432.oyvey.OyVey;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;

public class ImGuiScreen extends Screen {
    public ImGuiScreen() {
        super(Component.literal("ImGui"));
    }

    @Override
    public void render(GuiGraphics context, int mouseX, int mouseY, float delta) {
        OyVey.imGuiManager.render();
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }

    @Override
    public void renderBackground(GuiGraphics context, int mouseX, int mouseY, float delta) {
        // Do nothing to prevent the default background from rendering.
    }
}
