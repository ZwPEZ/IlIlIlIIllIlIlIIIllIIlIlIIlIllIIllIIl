package me.alpha432.oyvey.features.gui.items.buttons;

import me.alpha432.oyvey.OyVey;
import me.alpha432.oyvey.features.gui.OyVeyGui;
import me.alpha432.oyvey.features.gui.Widget;
import me.alpha432.oyvey.features.gui.items.Item;
import me.alpha432.oyvey.features.modules.client.ClickGui;
import me.alpha432.oyvey.util.render.RenderUtil;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.resources.sounds.SimpleSoundInstance;
import net.minecraft.sounds.SoundEvents;

public class Button
        extends Item {
    private boolean state;

    public Button(String name) {
        super(name);
        this.height = 15;
    }

    @Override
    public void drawScreen(GuiGraphics context, int mouseX, int mouseY, float partialTicks) {
        int color = this.getState()
                ? (this.isHovering(mouseX, mouseY)
                    ? OyVey.colorManager.getColorWithAlpha(y, ClickGui.getInstance().topColor.getValue().getAlpha())
                    : OyVey.colorManager.getColorWithAlpha(y, ClickGui.getInstance().color.getValue().getAlpha()))
                : (this.isHovering(mouseX, mouseY) ? 0x99333333 : 0x99111111);
        RenderUtil.drawSmoothRect(this.x, this.y, this.width, this.height - 0.5f, 3.5f, color);
        drawString(getName(), x + (width - mc.font.width(getName())) / 2f, this.y - 2.0f - (float) OyVeyGui.getClickGui().getTextOffset(), this.getState() ? -1 : -5592406);
    }

    @Override
    public void mouseClicked(int mouseX, int mouseY, int mouseButton) {
        if (mouseButton == 0 && this.isHovering(mouseX, mouseY)) {
            this.onMouseClick();
        }
    }

    public void onMouseClick() {
        this.state = !this.state;
        this.toggle();
        mc.getSoundManager().play(SimpleSoundInstance.forUI(SoundEvents.UI_BUTTON_CLICK, 1f));
    }

    public void toggle() {
    }

    public boolean getState() {
        return this.state;
    }

    @Override
    public int getHeight() {
        return 14;
    }

    public boolean isHovering(int mouseX, int mouseY) {
        for (Widget widget : OyVeyGui.getClickGui().getComponents()) {
            if (!widget.drag) continue;
            return false;
        }
        return (float) mouseX >= this.getX() && (float) mouseX <= this.getX() + (float) this.getWidth() && (float) mouseY >= this.getY() && (float) mouseY < this.getY() + (float) this.height;
    }
}