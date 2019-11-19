<template>
    <div class="b-slider">
        {{ label }}
        <div class="b-slider-track" ref="slider">
            <input class="b-slider-fill" type="range" :value="value" :max="max" @input="updateValue($event.target.value)" @change="updateValue($event.target.value)"/>
            <slot/>
        </div>
    </div>
</template>

<script>
    import SliderTick from './SliderTick'
    export default {
        name: 'BSlider',
        components: {
            [SliderTick.name]: SliderTick
        },
        props: {
            value: {
                type: Number,
                default: 0
            },
            label: String,
            min: {
                type: Number,
                default: 0
            },
            max: {
                type: Number,
                default: 100
            },
        },
        data() {
            return {
                isSlider: true // Used by Thumb and Tick
            }
        },
        computed: {

        },
        watch: {

        },
        methods: {
            updateValue: function (value) {
                this.$emit('input', parseInt(value))
            }
        },
        created() {

        }
    }
</script>
<style lang="scss">

    .b-slider-tick-label {
        font-size: 0.75rem;
        position: absolute;
        top: calc(#{1rem} / 2 + 2px);
        left: 50%;
        transform: translateX(-50%);
        text-align: center;
    }

    .b-slider {
        margin: 1em 0;
        background: transparent;
        width: 100%;
        flex: 1 1 auto;

        .b-slider-track {
            display: flex;
            align-items: center;
            position: relative;
            cursor: pointer;

        }

        .b-slider-fill {
            position: absolute;
            // Fix alignment in IE 11. Cancel out for others
            top: 50%;
            transform: translateY(-50%);
            width: 100%;
        }

        .b-slider-tick {
            position: absolute;
            width: 6px;
            height: 6px;
            border-radius: 3px;
            transform: translate(-50%, -50%);
            top: 50%;
            background: purple;

            &.is-tick-hidden {
                background: transparent;
            }
        }
    }
</style>