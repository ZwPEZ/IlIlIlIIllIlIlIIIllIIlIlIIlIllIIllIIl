package de.florianmichael.imguiexample.manager;

import de.florianmichael.imguiexample.features.modules.Module;
import de.florianmichael.imguiexample.features.modules.combat.Criticals;

import java.util.ArrayList;
import java.util.List;

public class ModuleManager {
    private final List<Module> modules = new ArrayList<>();

    public ModuleManager() {
        this.registerModules();
    }

    public List<Module> getModules() {
        return this.modules;
    }

    public List<Module> getModulesByCategory(Module.Category category) {
        List<Module> modules = new ArrayList<>();
        for (Module module : this.modules) {
            if (module.getCategory() == category) {
                modules.add(module);
            }
        }
        return modules;
    }

    private void registerModules() {
        this.modules.add(new Criticals());
    }
}
