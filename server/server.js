import express from 'express'
import cors from 'cors'
import { getDepartures } from './rheinbahn-times.js'

process.loadEnvFile('../.env')

const app = express()

app.use(cors())

let latestTime = null
const stopID = process.env?.STOP_ID ?? null
const directionID = process.env?.DIRECTION_ID ?? null
const lattitude = process.env?.LATTITUDE ?? null

async function updateTime () {
  const time = await getDepartures(stopID, directionID, lattitude)
  latestTime = time
}

async function startServer () {
  await updateTime()
  setInterval(updateTime, 15000)
  app.listen(8159, () => {
    console.log('server running on port 8159')
  })
}

startServer()

app.get('/nextTrain', (req, res) => {
  res.json({ nextTrainInMinutes: latestTime })
})
