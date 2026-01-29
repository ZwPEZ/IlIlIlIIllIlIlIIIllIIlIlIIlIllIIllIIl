package de.florianmichael.imguiexample;

import de.florianmichael.imguiexample.manager.ModuleManager;
import de.florianmichael.imguiexample.screens.ExampleScreen;
import net.fabricmc.api.ModInitializer;
import net.fabricmc.fabric.api.client.event.lifecycle.v1.ClientTickEvents;
import net.minecraft.client.KeyMapping;
import org.lwjgl.glfw.GLFW;

public class ExampleMod implements ModInitializer {
    public static final KeyMapping EXAMPLE_KEYBINDING = new KeyMapping(
            "key.imguiexample.example_keybinding",
            GLFW.GLFW_KEY_RIGHT_SHIFT,
            KeyMapping.Category.MISC
    );

    public static ModuleManager moduleManager;

    @Override
    public void onInitialize() {
        moduleManager = new ModuleManager();

        ClientTickEvents.END_CLIENT_TICK.register(client -> {
            if (EXAMPLE_KEYBINDING.consumeClick()) {
                client.setScreen(new ExampleScreen());
            }
        });
    }
}
