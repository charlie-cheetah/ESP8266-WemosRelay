<template>
    <v-container fill-height>
        <v-layout class="flex-column">
            <h1 class="subtitle">Laser</h1>

            <v-switch
                    v-model="switch1"
                    :label="switch1?'On':'Off'"

            ></v-switch>

            <v-select v-model="ch1" @input="sendData" :items="ch1presets"
                      item-text="name"
                      item-value="id">
            </v-select>

            <b-slider label="Pattern"
                      thumb-label
                      v-model="ch2" :max="255" ></b-slider>

            <b-slider v-model="ch3"
                      label="Rotation"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed clockwise</b-slider-tick>
                <b-slider-tick :value="192">Speed counterclockwise</b-slider-tick>
            </b-slider>
            <b-slider v-model="ch4"
                      label="Flip horizontal"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>

            <b-slider v-model="ch5"
                      label="Flip vertical"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Angle</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>

            <b-slider v-model="ch6"
                      label="Position vertical"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>

            <b-slider v-model="ch7"
                      label="Position horizontal"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Position</b-slider-tick>
                <b-slider-tick :value="128">Speed</b-slider-tick>
            </b-slider>

            <b-slider v-model="ch8"
                      label="Size"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Fixed</b-slider-tick>
                <b-slider-tick :value="64">Ascending speed</b-slider-tick>
                <b-slider-tick :value="128">Descending speed</b-slider-tick>
                <b-slider-tick :value="192">Scaling speed</b-slider-tick>
            </b-slider>

            <b-slider v-model="ch9"
                      label="Color"
                      :max="255"
                      @input="sendData"
                      @change="sendData">
                <b-slider-tick :value="0">Purple</b-slider-tick>
                <b-slider-tick :value="20">Red</b-slider-tick>
                <b-slider-tick :value="40">Purple</b-slider-tick>
                <b-slider-tick :value="60">Purple</b-slider-tick>
                <b-slider-tick :value="80">Purple</b-slider-tick>
                <b-slider-tick :value="100">Purple</b-slider-tick>
                <b-slider-tick :value="120">Purple</b-slider-tick>
                <b-slider-tick :value="128">Cycle speed</b-slider-tick>
            </b-slider>

            <v-btn color="primary" @click="addPreset">Add to sequence</v-btn>


                <v-card class="mt-2">
                    <v-card-text>
                        <div class="d-flex flex-wrap">
                            <p class="pa-2">Sequences:</p>
                            <div class="mr-2" v-for="(item,index) in sequences" :key="'seq'+index">
                                <v-btn color="teal" v-if="index===currentSequence">{{item.name}} ({{item.presets.length}})</v-btn>
                                <v-btn v-else @click="changeSequence(index)">{{item.name}} ({{item.presets.length}})</v-btn>
                            </div>
                            <v-btn @click="addNewSequence">+</v-btn>
                        </div>
                        <div v-if="rename">
                            <v-text-field
                                    label="Name"
                                    v-model="sequenceName"
                                    @change="renameSequence"
                            ></v-text-field>
                        </div>
                    </v-card-text>
                    <v-card-actions>
                        <v-btn color="primary" small @click="saveStorage">Save to storage</v-btn>
                        <v-btn color="primary" small @click="enableSequenceRename">Rename sequence</v-btn>
                        <v-btn color="error" small @click="deleteSequence">Delete sequence</v-btn>
                    </v-card-actions>
                </v-card>

                <v-row>
                    <v-col>
                        <div class="d-flex justify-space-between pa-2 elevation-4 mb-2" :class="i===currentPreset?'teal':'secondary'" @click="selectPreset(i)" v-for="(pre,i) in sequences[currentSequence].presets" :key="'pre'+i">
                            <v-chip :color="pre.values[0]===1?'error':'orange'">
                                {{pre.values[0]===1?'Off':''}}
                                {{pre.values[0]===100?'M':''}}
                            </v-chip>

                            <div>{{pre.values[1]}}</div>
                            <v-chip color="orange"><v-icon>mdi-rotate-left</v-icon>{{pre.values[2]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-arrow-expand-vertical</v-icon>{{pre.values[3]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-arrow-expand-horizontal</v-icon>{{pre.values[4]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-swap-vertical</v-icon>{{pre.values[5]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-swap-horizontal</v-icon>{{pre.values[6]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-arrow-expand-all</v-icon>{{pre.values[6]}}</v-chip>
                            <v-chip color="orange"><v-icon>mdi-palette</v-icon>{{pre.values[6]}}</v-chip>

                            <v-btn color="error" @click="removePreset(i)">delete</v-btn>
                        </div>
                    </v-col>
                    <v-col>
                        <v-card class="mx-auto">
                            <v-toolbar flat dense>
                                <v-toolbar-title>
                                    <span class="subheading">METRONOME</span>
                                </v-toolbar-title>
                            </v-toolbar>

                            <v-card-text>
                                <v-row class="mb-4" justify="space-between">
                                    <v-col class="text-left">
                                        <span class="display-3 font-weight-light" v-text="bpm"></span>
                                        <span class="subheading font-weight-light mr-1">BPM</span>
                                    </v-col>
                                    <v-col class="text-right">
                                        <v-btn color="primary" dark depressed fab @click="toggle">
                                            <v-icon large>{{ isPlaying ? 'mdi-pause' : 'mdi-play' }}</v-icon>
                                        </v-btn>
                                    </v-col>
                                </v-row>

                                <v-slider
                                        v-model="bpm"
                                        color="primary"
                                        track-color="grey"
                                        always-dirty
                                        min="40"
                                        max="218"
                                >
                                    <template v-slot:prepend>
                                        <v-icon @click="decrement">mdi-minus</v-icon>
                                    </template>

                                    <template v-slot:append>
                                        <v-icon @click="increment">mdi-plus</v-icon>
                                    </template>
                                </v-slider>
                            </v-card-text>
                        </v-card>
                    </v-col>
                </v-row>
        </v-layout>
    </v-container>
</template>

<script>
    import _ from 'lodash'
    import BSlider from '../components/Slider.vue'
    import BSliderTick from '../components/SliderTick.vue'

    export default {
        name: "Laser",
        components: { BSlider,BSliderTick},
        data() {
            return {
                switch1: false,
                response: '',
                ch1: 1,
                ch2: 0,
                ch3: 0,
                ch4: 0,
                ch5: 0,
                ch6: 0,
                ch7: 0,
                ch8: 0,
                ch9: 0,
                rename: false,
                sequenceName: 'unnamed',
                ch1presets: [
                    {'id':1,'name':'Off'},
                    {'id':100,'name':'Manual'},
                    {'id':150,'name':'Auto'},
                    {'id':220,'name':'Auto - Sound'},
                ],
                currentSequence: 0,
                currentPreset: 0,
                sequences: [
                    {name:'unnamed',
                        presets:[
                            {values:[1,0,0,0,0,0,0,0]},
                            {values:[1,0,0,0,0,0,0,0]}
                        ]
                    },
                ],
                bpm: 40,
                metronome: null,
                isPlaying: false,
            }
        },
        mounted: function () {
            this.$connect('ws://' +  this.$location + '/ws');
            let saved = localStorage.getItem('sequences');
            if (saved !== null) {
                this.sequences = JSON.parse(localStorage.getItem('sequences'));
            }
        },
        computed: {
            animationDuration () {
                return `${60 / this.bpm}s`
            },
        },
        methods: {
            decrement () {
                this.bpm--
            },
            increment () {
                this.bpm++
            },
            toggle () {
                this.isPlaying = !this.isPlaying;
                if (this.isPlaying) {
                    this.metronome = new window.Metronome(this.bpm,100);
                    this.metronome.start();
                } else {
                    this.metronome.stop();
                }

            },
            saveStorage: function() {
                localStorage.setItem('sequences', JSON.stringify(this.sequences));
            },
            enableSequenceRename() {
                this.rename = true;
                this.sequenceName = this.sequences[this.currentSequence].name;
            },
            renameSequence: function(val) {
                this.sequences[this.currentSequence].name = val;
                this.rename = false;
            },
            addPreset: function() {
                this.sequences[this.currentSequence].presets.splice(this.currentPreset,0,{values:[this.ch1,this.ch2,this.ch3,this.ch4,this.ch5,this.ch6,this.ch7,this.ch8]})
            },
            addNewSequence: function() {
                this.sequences.push({name:'unnamed',presets:[]});
            },
            changeSequence: function(index) {
                this.currentSequence = index;
                this.rename=false;
            },
            removePreset: function(index) {
                this.sequences[this.currentSequence].presets.splice(index,1);
            },
            selectPreset: function(index) {
                this.currentPreset = index;
                for (const x in this.sequences[this.currentSequence].presets[index].values) {
                    this['ch'+x] = this.sequences[this.currentSequence].presets[index].values[x];
                }
            },
            deleteSequence: function() {
                let tmp = this.currentSequence;
                this.currentSequence--;
                this.sequences.splice(tmp,1);
            },
            turnOn: function() {
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