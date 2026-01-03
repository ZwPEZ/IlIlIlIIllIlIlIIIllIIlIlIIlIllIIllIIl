package me.alpha432.oyvey.manager;

import imgui.ImGui;
import imgui.gl3.ImGuiImplGl3;
import imgui.glfw.ImGuiImplGlfw;
import imgui.type.ImBoolean;
import me.alpha432.oyvey.OyVey;
import me.alpha432.oyvey.features.modules.Module;
import org.lwjgl.opengl.GL11;
import me.alpha432.oyvey.util.traits.Util;

public class ImGuiManager implements Util {
    private final ImGuiImplGlfw implGlfw = new ImGuiImplGlfw();
    private final ImGuiImplGl3 implGl3 = new ImGuiImplGl3();

    private long windowHandle;

    public void init() {
        ImGui.createContext();
        windowHandle = mc.getWindow().handle();
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

        GL11.glEnable(GL11.GL_BLEND);
        GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
        GL11.glDisable(GL11.GL_DEPTH_TEST);
        GL11.glDisable(GL11.GL_CULL_FACE);

        implGl3.renderDrawData(ImGui.getDrawData());

        GL11.glEnable(GL11.GL_DEPTH_TEST);
    }

    public void destroy() {
        implGl3.dispose();
        implGlfw.dispose();
        ImGui.destroyContext();
    }
}
