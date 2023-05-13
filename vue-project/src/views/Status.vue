<template>
<div>
  <el-row type="flex">
    <el-col :span="24">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>System</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid3'>Model</div>
          <div class='grid7 text-right'>{{ data?.sys?.model }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Architecture</div>
          <div class='grid7 text-right'>{{ data?.sys?.arch.mfr + ' ' +
            data?.sys?.arch.model + ' ' +
            "rev " + data?.sys?.arch.revision + ' ' +
            data?.sys?.arch.cpu + ' @ ' +
            data?.sys?.arch.freq + 'Mhz' }}
          </div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Firmware version</div>
          <div class='grid7 text-right'>{{ data?.sys?.fw }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>SDK version</div>
          <div class='grid7 text-right'>{{ data?.sys?.sdk }}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>

  <el-row type="flex">
    <el-col :span="24">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>Memory</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid3'>Total Availabl</div>
          <div class='grid7 text-right'>{{ (data?.mem?.total / 1024).toFixed(2) + " KB" }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Free</div>
          <div class='grid7 text-right'>{{ (data?.mem?.free / 1024).toFixed(2) + " KB" }}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>

  <el-row>
    <el-col :span="24">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>File System </span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid3'>Total Availabl</div>
          <div class='grid7 text-right'>{{ (data?.fs?.total / 1024 / 1024).toFixed(2) + " MB" }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Used</div>
          <div class='grid7 text-right'>{{ (data?.fs?.used / 1024 / 1024).toFixed(2) + " MB" }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Free</div>
          <div class='grid7 text-right'>{{ (data?.fs?.free / 1024 / 1024).toFixed(2) + " MB" }}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>

  <el-row>
    <el-col :span="24">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>Wireless (AP)</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid3'>SSID</div>
          <div class='grid7 text-right'>{{ data?.ap?.ssid }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Number of associations</div>
          <div class='grid7 text-right'>{{ data?.ap?.num }}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>

  <el-row>
    <el-col :span="24">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>Wireless (STA)</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid3'>SSID</div>
          <div class='grid7 text-right'>{{ data?.sta?.ssid }}</div>
        </div>
        <div class="text item flex">
          <div class='grid3'>Status</div>
          <div class='grid7 text-right'>{{ data?.sta?.status }}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>
</div>
</template>

<script lang="ts">
import axios from 'axios'

export default {
  data() {
    return {
      data: {
        sys: {
          model: null,
          fw: null,
          sdk: null,
          arch: {
            mfr: null,
            model: null,
            revision: null,
            cpu: null,
            freq: null
          }
        },
        mem: {
          total: 0,
          free: 0
        },
        fs: {
          total: 0,
          used: 0,
          free: 0
        },
        ap: {
          ssid: null,
          num: null
        },
        sta: {
          ssid: null,
          status: null
        }
      }
    }
  },
  mounted() {
    axios
      .get('/api/v1/status')
      .then(response => (this.data = response.data))
      .catch(function (error) {
        console.log(error);
      });
  }
}
</script>

<style scoped lang="scss">
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.text {
  font-size: 14px;
}

.item {
  margin-bottom: 18px;
}

.box-card {
  width: 100%;
  margin-bottom: 20px;
}

.flex {
  display: flex;
}

.grid3 {
  width: 30%;
}

.grid5 {
  width: 50%;
}

.grid7 {
  width: 70%;
}

.text-right {
  text-align: left;
}
</style>
