package me.alpha432.oyvey.manager;

import imgui.ImGui;
import imgui.gl3.ImGuiImplGl3;
import imgui.glfw.ImGuiImplGlfw;
import imgui.type.ImBoolean;
import me.alpha432.oyvey.OyVey;
import me.alpha432.oyvey.features.modules.Module;
import me.alpha432.oyvey.util.Util;
import net.minecraft.client.MinecraftClient;

public class ImGuiManager implements Util {
    private final ImGuiImplGlfw implGlfw = new ImGuiImplGlfw();
    private final ImGuiImplGl3 implGl3 = new ImGuiImplGl3();

    private long windowHandle;

    public void init() {
        ImGui.createContext();
        windowHandle = MinecraftClient.getInstance().getWindow().getHandle();
        implGlfw.init(windowHandle, true);
        implGl3.init();
    }

    public void render() {
        implGlfw.newFrame();
        ImGui.newFrame();

        for (Module.Category category : OyVey.moduleManager.getCategories()) {
            if (category == Module.Category.HUD) continue;

            ImGui.begin(category.getName());
            for (Module module : OyVey.moduleManager.getModulesByCategory(category)) {
                ImBoolean enabled = new ImBoolean(module.isEnabled());
                if (ImGui.checkbox(module.getName(), enabled)) {
                    module.toggle();
                }
            }
            ImGui.end();
        }

        ImGui.endFrame();
        ImGui.render();
        implGl3.renderDrawData(ImGui.getDrawData());
    }

    public void destroy() {
        implGl3.dispose();
        implGlfw.dispose();
        ImGui.destroyContext();
    }
}
