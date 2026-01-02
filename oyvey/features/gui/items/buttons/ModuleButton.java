package me.alpha432.oyvey.features.gui.items.buttons;

import me.alpha432.oyvey.features.gui.items.Item;
import me.alpha432.oyvey.features.modules.Module;
import me.alpha432.oyvey.features.settings.Bind;
import me.alpha432.oyvey.features.settings.Setting;
import me.alpha432.oyvey.util.render.Animation;
import me.alpha432.oyvey.util.render.ScissorUtil;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.resources.sounds.SimpleSoundInstance;
import net.minecraft.sounds.SoundEvents;

import java.util.ArrayList;
import java.util.List;

public class ModuleButton
        extends Button {
    private final Module module;
    private List<Item> items = new ArrayList<>();
    private boolean subOpen;
    private final Animation animation = new Animation(200, 0);

    public ModuleButton(Module module) {
        super(module.getName());
        this.module = module;
        this.initSettings();
    }

    public void initSettings() {
        ArrayList<Item> newItems = new ArrayList<>();
        if (!this.module.getSettings().isEmpty()) {
            for (Setting<?> setting : this.module.getSettings()) {
                if (setting.getValue() instanceof Boolean && !setting.getName().equals("Enabled")) {
                    newItems.add(new BooleanButton((Setting<Boolean>) setting));
                }
                if (setting.getValue() instanceof Bind && !setting.getName().equalsIgnoreCase("Keybind") && !this.module.getName().equalsIgnoreCase("Hud")) {
                    newItems.add(new BindButton((Setting<Bind>) setting));
                }
                if ((setting.getValue() instanceof String || setting.getValue() instanceof Character) && !setting.getName().equalsIgnoreCase("displayName")) {
                    newItems.add(new StringButton((Setting<String>) setting));
                }
                if (setting.isColorSetting()) {
                    newItems.add(new ColorButton((Setting<java.awt.Color>) setting));
                    continue;
                }
                if (setting.isNumberSetting() && setting.hasRestriction()) {
                    newItems.add(new Slider((Setting<Number>) setting));
                    continue;
                }
                if (!setting.isEnumSetting()) continue;
                newItems.add(new EnumButton((Setting<Enum<?>>) setting));
            }
        }
        newItems.add(new BindButton((Setting<Bind>) this.module.getSettingByName("Keybind")));
        this.items = newItems;
    }

    @Override
    public void drawScreen(GuiGraphics context, int mouseX, int mouseY, float partialTicks) {
        super.drawScreen(context, mouseX, mouseY, partialTicks);
        animation.setEnd(subOpen ? 1 : 0);
        animation.update();
        if (!this.items.isEmpty() && animation.getCurrent() > 0) {
            float itemHeight = 0;
            for (Item item : items)
                itemHeight += item.getHeight() + 1.5f;

            ScissorUtil.enable(context, (int) x, (int) y + 14, (int) width, (int) (itemHeight * animation.getCurrent()));
            float height = 14.0f;
            for (Item item : this.items) {
                if (!item.isHidden()) {
                    item.setLocation(this.x + 1.0f, this.y + height);
                    item.setWidth(this.width - 9);
                    item.drawScreen(context, mouseX, mouseY, partialTicks);
                    height += item.getHeight() + 1.5f;
                }
                item.update();
            }
            ScissorUtil.disable(context);
        }
    }

    @Override
    public void mouseClicked(int mouseX, int mouseY, int mouseButton) {
        super.mouseClicked(mouseX, mouseY, mouseButton);
        if (!this.items.isEmpty()) {
            if (mouseButton == 1 && this.isHovering(mouseX, mouseY)) {
                this.subOpen = !this.subOpen;
                mc.getSoundManager().play(SimpleSoundInstance.forUI(SoundEvents.UI_BUTTON_CLICK, 1f));
            }
            if (animation.getCurrent() > 0.01) {
                for (Item item : this.items) {
                    if (item.isHidden()) continue;
                    item.mouseClicked(mouseX, mouseY, mouseButton);
                }
            }
        }
    }

    @Override
    public void mouseReleased(int mouseX, int mouseY, int releaseButton) {
        super.mouseReleased(mouseX, mouseY, releaseButton);
        if (animation.getCurrent() > 0.01) {
            for (Item item : this.items) {
                if (item.isHidden()) continue;
                item.mouseReleased(mouseX, mouseY, releaseButton);
            }
        }
    }

    @Override
    public void onKeyTyped(String typedChar, int keyCode) {
        super.onKeyTyped(typedChar, keyCode);
        if (animation.getCurrent() > 0.01) {
            for (Item item : this.items) {
                if (item.isHidden()) continue;
                item.onKeyTyped(typedChar, keyCode);
            }
        }
    }

    @Override
    public void onKeyPressed(int key) {
        super.onKeyPressed(key);
        if (animation.getCurrent() > 0.01) {
            for (Item item : this.items) {
                if (item.isHidden()) continue;
                item.onKeyPressed(key);
            }
        }
    }

    @Override
    public int getHeight() {
        if (subOpen) {
            float height = 14.0f;
            for (Item item : this.items) {
                if (item.isHidden()) continue;
                height += item.getHeight() + 1.5f;
            }
            return (int) height;
        }
        return 14;
    }

    public Module getModule() {
        return this.module;
    }

    @Override
    public void toggle() {
        this.module.toggle();
    }

    @Override
    public boolean getState() {
        return this.module.isEnabled();
    }
}