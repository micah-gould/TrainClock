import express from "express"
import cors from "cors"
import { getDepartures } from "./rheinbahn-times.mjs"

const app = express()

app.use(cors())

let latestTime = null
const stopID = 653379
const directionID = 653647

async function updateTime() {
  const time = await getDepartures(stopID, directionID)
  latestTime = time
}

async function startServer() {
  await updateTime()
  setInterval(updateTime, 15000)
  app.listen(8159, () => {
    console.log("server running on port 8159")
  })
}

startServer()

app.get("/nextTrain", (req, res) => {
  res.json({ nextTrainInMinutes: latestTime})
})
