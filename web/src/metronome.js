
/*!
 * Metronome.js
 *
 * A reliable metronome using HTML5 Web Workers to maintain the tick
 * interval even when the parent thread is run in the background.
 *
 * For details, see:
 * http://pivotallabs.com/chrome-and-firefox-throttle-settimeout-setinterval-in-inactive-tabs/
 *
 * Copyright 2012 Nick Thompson
 * MIT License
 */

(function (window) {
    let timer;
    var msgHandler = function (e) {
        if (e.data.action === "start") {
            var freq = (60000 / e.data.bpm) / e.data.res;
            if (e.data.reduce) { freq = freq - (freq * 0.1); }
            timer = setInterval(self.postMessage, freq);
        } else if (e.data.action === "stop") {
            clearInterval(timer);
        }
    }

        , inline = "" +
        "self.addEventListener('message', " + msgHandler.toString() + ");";

    /**
     * Metronome class constructor.
     *
     * @param {number} bpm Beats per minute
     * @param {number} res Resolution; metronome ticks per beat
     */
    function Metronome (bpm, res) {
        this.bpm = bpm;
        this.res = res;
        this.ticks = 0;

        var that = this
            , url = window.URL || window.webkitURL
            , blob = new Blob([inline])
            , blobUrl = url.createObjectURL(blob);

        this.worker = new Worker(blobUrl);

        this.worker.addEventListener("message", function () {
            that.ticks++;
            that.trigger("tick", that.ticks);
            that.trigger(that.ticks);
        });

    }

    /**
     * Add an event listener to the Metronome.
     *
     * On every tick of the metrome, listeners bound to the "tick"
     * event will be called with a single argument which is the number
     * of times the metronome has ticked so far.
     *
     * Additionally, on the nth tick of the metronome, listeners bound
     * to the event `n` (number) will be called.
     *
     * @param {string|number} e
     * @param {function} listener
     */
    Metronome.prototype.on = function (e, listener) {
        this._events = this._events || {};
        this._events[e] = this._events[e] || [];
        this._events[e].push(listener);
    };

    /**
     * Remove an event listener.
     *
     * @param {string|number} e
     * @param {function} listener
     */
    Metronome.prototype.removeListener = function (e, listener) {
        this._events = this._events || {};
        if (e in this._events) {
            this._events[e].splice(this._events[e].indexOf(listener), 1);
        }
    };

    /**
     * Call each listener bound to a given event with the supplied arguments.
     *
     * @param {string|number} e
     * @param {object} args Array of arguments to apply to the listeners.
     * @api private
     */
    Metronome.prototype.trigger = function (e) {
        this._events = this._events || {};
        if (e in this._events === false) { return; }
        for (var i = this._events[e].length; i--;) {
            this._events[e][i].apply(this, Array.prototype.slice.call(arguments, 1));
        }
    };

    /**
     * Start the metronome.
     */
    Metronome.prototype.start = function () {
        this.worker.postMessage({
            action: "start",
            bpm: this.bpm,
            res: this.res,
        });
    };

    /**
     * Stop the metronome.
     */
    Metronome.prototype.stop = function () {
        this.worker.postMessage({
            action: "stop"
        });
    };

    /**
     * Reset the tick count.
     */
    Metronome.prototype.reset = function () {
        this.ticks = 0;
    };

    window.Metronome = Metronome;

})(this);