import express from "express"
import cors from "cors"
import { getDepartures } from "./rheinbahn-times.mjs"

const app = express()

app.use(cors())

let latestTime = null
const id = 653379

async function updateTime() {
  const time = await getDepartures(id)
  latestTime = time
}

setInterval(updateTime, 15000)
updateTime()

app.get("/nextTrain", (req, res) => {
  res.json({ nextTrainInMinutes: latestTime})
})

app.listen(8159, () => {
  console.log("server running on port 8159")
})
