<template>
    <div class="container">
        <h1 class="subtitle">Laser</h1>
        <div class="buttons">
            <b-button :type="relay===1?'is-success':'is-light'" @click="turnOn">On</b-button>
            <b-button :type="relay===0?'is-danger':'is-light'" @click="turnOff">Off</b-button>
        </div>

        {{response}}

        <b-field label="Pattern" class="">
            <b-select v-model="ch1" @input="sendData">
                <option
                        v-for="option in data"
                        :value="option.id"
                        :key="option.id">
                    {{ option.name }}
                </option>
            </b-select>
        </b-field>

        <b-field label="Pattern" class="">
            <b-slider v-model="ch2" :max="255" @dragstart="startInterval" @dragend="stopInterval" @change="sendData"></b-slider>
        </b-field>

        <b-field label="Rotation" class="p-t-md">
            <b-slider v-model="ch3" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed clockwise</b-slider-tick>
                <b-slider-tick :value="192">Speed counterclockwise</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Flip horizontal" class="p-t-md">
            <b-slider v-model="ch4" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Flip vertical" class="p-t-md">
            <b-slider v-model="ch5" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Position vertical" class="p-t-md">
            <b-slider v-model="ch6" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Position horizontal" class="p-t-md">
            <b-slider v-model="ch7" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Size" class="p-t-md">
            <b-slider v-model="ch8" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Fixed</b-slider-tick>
                <b-slider-tick :value="64">Ascending speed</b-slider-tick>
                <b-slider-tick :value="128">Descending speed</b-slider-tick>
                <b-slider-tick :value="192">Scaling speed</b-slider-tick>
            </b-slider>
        </b-field>

        <b-field label="Color" class="p-t-md">
            <b-slider v-model="ch9" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Purple</b-slider-tick>
                <b-slider-tick :value="20">Red</b-slider-tick>
                <b-slider-tick :value="40">Purple</b-slider-tick>
                <b-slider-tick :value="60">Purple</b-slider-tick>
                <b-slider-tick :value="80">Purple</b-slider-tick>
                <b-slider-tick :value="100">Purple</b-slider-tick>
                <b-slider-tick :value="120">Purple</b-slider-tick>
                <b-slider-tick :value="128">Cycle speed</b-slider-tick>
            </b-slider>
        </b-field>


    </div>
</template>

<script>
    export default {
        name: "Laser",
        data() {
            return {
                relay: 0,
                response: '',
                interval: false,
                ch1: 1,
                ch2: 0,
                ch3: 0,
                ch4: 0,
                ch5: 0,
                ch6: 0,
                ch7: 0,
                ch8: 0,
                ch9: 0,
                data: [
                    {'id':1,'name':'Off'},
                    {'id':100,'name':'Manual'},
                    {'id':150,'name':'Auto'},
                    {'id':220,'name':'Auto - Sound'},
                ]
            }
        },
        mounted: function () {
            let url = new URL('http://' + window.location.host + '/artnet');
            fetch(url, {

            }).then(function(response){
                this.response = response
            });
        },
        methods: {
            turnOn: function() {
                let url = new URL('http://' + window.location.host + '/on');
                fetch(url, {

                }).then(function(response){
                    this.response = response
                });
            },
            turnOff: function() {
                let url = new URL('http://' + window.location.host + '/off');
                fetch(url, {

                }).then(function(response){
                    this.response = response
                });
            },
            startInterval: function() {
                this.interval = setInterval(function(){ this.sendData(); }.bind(this),100);
            },
            stopInterval: function() {
                clearInterval(this.interval);
            },
            sendData: function() {
                let url = new URL('http://' + window.location.host + '/artnet');
                let params = {
                    0:this.ch1,
                    1:this.ch2,
                    2:this.ch3,
                    3:this.ch4,
                    4:this.ch5,
                    5:this.ch6,
                    6:this.ch7,
                    7:this.ch8,
                    8:this.ch9
                    }
                Object.keys(params).forEach(key => url.searchParams.append(key, params[key]))
                fetch(url, {
                    method: 'post',
                }).then(function(response){
                    this.response = response
                });
            }
        }
    }
</script>

<style scoped>
    .p-t-md { padding-top: 1rem; }
</style>