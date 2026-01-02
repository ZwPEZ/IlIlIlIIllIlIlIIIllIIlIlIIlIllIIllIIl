package me.alpha432.oyvey.util.render;

public class Animation {
    private double start = 0.0;
    private double end = 0.0;
    private final long duration;
    private long startTime;
    private double current = 0.0;

    public Animation(long duration, double end) {
        this.duration = duration;
        this.end = end;
        this.current = end;
        this.startTime = System.currentTimeMillis();
    }

    public void setEnd(double end) {
        if (this.end != end) {
            this.startTime = System.currentTimeMillis();
            this.start = this.current;
            this.end = end;
        }
    }

    public void update() {
        long now = System.currentTimeMillis();
        long elapsedTime = now - this.startTime;

        if (elapsedTime < this.duration) {
            double progress = (double) elapsedTime / this.duration;
            this.current = this.start + (this.end - this.start) * this.easeOutCubic(progress);
        } else {
            this.current = this.end;
        }
    }

    private double easeOutCubic(double x) {
        return 1 - Math.pow(1 - x, 3);
    }

    public double getCurrent() {
        return this.current;
    }

    public void setCurrent(double current) {
        this.current = current;
        this.start = current;
        this.end = current;
        this.startTime = System.currentTimeMillis();
    }
}
