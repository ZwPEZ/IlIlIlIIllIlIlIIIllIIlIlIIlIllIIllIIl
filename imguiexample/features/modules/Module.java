package de.florianmichael.imguiexample.features.modules;

import de.florianmichael.imguiexample.features.Feature;

public class Module extends Feature {
    private final String description;
    private final Category category;
    private final Section section;
    private boolean enabled;
    private boolean hidden;
    private int bind;
    private String displayName;

    public Module(String name, String description, Category category, Section section) {
        super(name);
        this.displayName = name;
        this.description = description;
        this.category = category;
        this.section = section;
        this.enabled = false;
        this.hidden = false;
        this.bind = -1;
    }

    public void onEnable() {
    }

    public void onDisable() {
    }

    public void onToggle() {
    }

    public void onLoad() {
    }

    public void onTick() {
    }

    public String getDisplayInfo() {
        return null;
    }

    public void setEnabled(boolean enabled) {
        if (enabled) {
            this.enable();
        } else {
            this.disable();
        }
    }

    public void enable() {
        this.enabled = true;
        this.onToggle();
        this.onEnable();
    }

    public void disable() {
        this.enabled = false;
        this.onToggle();
        this.onDisable();
    }

    public void toggle() {
        this.setEnabled(!this.isEnabled());
    }

    public String getDisplayName() {
        return this.displayName;
    }

    public void setDisplayName(String name) {
        this.displayName = name;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public String getDescription() {
        return this.description;
    }

    public boolean isHidden() {
        return this.hidden;
    }

    public void setHidden(boolean hidden) {
        this.hidden = hidden;
    }

    public Category getCategory() {
        return this.category;
    }

    public Section getSection() {
        return this.section;
    }

    public int getBind() {
        return this.bind;
    }

    public void setBind(int key) {
        this.bind = key;
    }

    public enum Category {
        COMBAT("Combat"),
        MISC("Misc"),
        RENDER("Render"),
        MOVEMENT("Movement"),
        PLAYER("Player"),
        CLIENT("Client");

        private final String name;

        Category(String name) {
            this.name = name;
        }

        public String getName() {
            return this.name;
        }
    }
}
