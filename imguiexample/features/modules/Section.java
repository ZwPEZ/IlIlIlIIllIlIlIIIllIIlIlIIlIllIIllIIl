package de.florianmichael.imguiexample.features.modules;

public enum Section {
    LEFT("Left"),
    MIDDLE("Middle"),
    RIGHT("Right");

    private final String name;

    Section(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}
