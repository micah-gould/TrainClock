import express from 'express'
import cors from 'cors'
import dotenv from 'dotenv'
import { getDepartures } from './rheinbahn-times.js'
import path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

dotenv.config({ path: path.resolve(__dirname, '../.env') })

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
