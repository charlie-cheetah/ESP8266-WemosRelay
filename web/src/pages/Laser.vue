<template>
    <v-container fill-height>
        <v-layout class="flex-column">
            <h1 class="subtitle">Laser</h1>

                <v-switch
                        v-model="switch1"
                        :label="switch1?'On':'Off'"

                ></v-switch>



            {{response}}

            <v-select v-model="ch1" @input="sendData" :items="data"
                      item-text="name"
                      item-value="id">
            </v-select>


            <v-subheader>Pattern</v-subheader>

            <v-slider label="Pattern"
                      thumb-label
                      v-model="ch2" :max="255" @end="sendData" @change="sendData"></v-slider>

            <v-slider v-model="ch3"
                      thumb-label
                      label="Rotation"
                      :max="255"
                      @dragging="sendData"
                      @change="sendData"
                      :tick-labels="rotationLabels">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed clockwise</b-slider-tick>
                <b-slider-tick :value="192">Speed counterclockwise</b-slider-tick>
            </v-slider>

            <v-subheader>Flip horizontal</v-subheader>

            <v-slider v-model="ch4" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </v-slider>

            <v-subheader>Flip vertical</v-subheader>

            <v-slider v-model="ch5" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </v-slider>

            <v-subheader>Position vertical</v-subheader>

            <v-slider v-model="ch6" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </v-slider>

            <v-subheader>Position horizontal</v-subheader>

            <v-slider v-model="ch7" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </v-slider>

            <v-subheader>Size</v-subheader>

            <v-slider v-model="ch8" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Fixed</b-slider-tick>
                <b-slider-tick :value="64">Ascending speed</b-slider-tick>
                <b-slider-tick :value="128">Descending speed</b-slider-tick>
                <b-slider-tick :value="192">Scaling speed</b-slider-tick>
            </v-slider>

            <v-subheader>Color</v-subheader>

            <v-slider v-model="ch9" :max="255" @dragging="sendData" @change="sendData">
                <b-slider-tick :value="0">Purple</b-slider-tick>
                <b-slider-tick :value="20">Red</b-slider-tick>
                <b-slider-tick :value="40">Purple</b-slider-tick>
                <b-slider-tick :value="60">Purple</b-slider-tick>
                <b-slider-tick :value="80">Purple</b-slider-tick>
                <b-slider-tick :value="100">Purple</b-slider-tick>
                <b-slider-tick :value="120">Purple</b-slider-tick>
                <b-slider-tick :value="128">Cycle speed</b-slider-tick>
            </v-slider>

            <v-btn @click="addSequence">Add to sequence</v-btn>
        </v-layout>
    </v-container>
</template>

<script>
    import _ from 'lodash'

    export default {
        name: "Laser",
        data() {
            return {
                switch1: false,
                response: '',
                interval: false,
                rotationLabels: [''],
                rotationLabels2: [''],
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
            this.$connect('ws://' +  this.$location + '/ws');
            this.rotationLabels = [null];
            this.rotationLabels[100] = 'Summer';
            //this.rotationLabels[1] = 'b';

            console.log(this.rotationLabels);
        },
        methods: {
            addSequence: function() {

            },
            turnOn: function() {
                console.log(this.$location);
                let url = new URL('http://' + this.$location + '/relayon');
                fetch(url, {

                }).then(function(response){
                    this.response = response
                });
            },
            turnOff: function() {
                let url = new URL('http://' + this.$location + '/relayoff');
                fetch(url, {

                }).then(function(response){
                    this.response = response
                });
            },
            sendData: function() {
                if (this.$socket.readyState === WebSocket.OPEN) this.sendThrottled();
            },
            sendThrottled: _.throttle(function () {
                const bytesArray = [
                    this.ch1,
                    this.ch2,
                    this.ch3,
                    this.ch4,
                    this.ch5,
                    this.ch6,
                    this.ch7,
                    this.ch8,
                    this.ch9,
                ];
                let bytestream = Uint8Array.from(bytesArray);
                if (this.$socket)
                    this.$socket.send(bytestream);
            }, 25)
        }
    }
</script>

<style scoped>
    .p-t-md { padding-top: 1rem; }
</style>