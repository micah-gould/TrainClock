import fetch from 'node-fetch'

const DEFAULT_STOP_ID = 653379
const DEFAULT_DIRECTION_ID = 653647
const DEFAULT_LATITUDE = 51.312882

const delay = ms => new Promise(r => setTimeout(r, ms))

export async function getDepartures(
  stopID = DEFAULT_STOP_ID,
  directionID = DEFAULT_DIRECTION_ID,
  count = 0
) {
  try {
    const URL = `https://v6.db.transport.rest/stops/${stopID}/departures?results=2&direction=${directionID}`
    const response = await fetch(URL)
    const json = await response.json()

    const departures = json?.departures ?? []

    const filtered = departures.filter(
      d => Math.abs(d.stop.location.latitude - DEFAULT_LATITUDE) < 0.00001
    )

    const getTime = departure => {
      if (!departure?.when && !departure?.plannedWhen) return -1
      const date = new Date(departure.when || departure.plannedWhen)
      return Math.round((date - Date.now()) / 60000)
    }

    const next = getTime(filtered[0])
    return next > 0 ? next : getTime(filtered[1])

  } catch (e) {
    console.error(e)
    if (count >= 5) return -1
    await delay(1000)
    return getDepartures(stopID, directionID, count + 1)
  }
}

async function Main() {
  const [stopID, directionID] = process.argv.slice(2)
  console.log(await getDepartures(stopID, directionID))
}

const isMain =
  process.argv[1] &&
  new URL(import.meta.url).pathname === process.argv[1]

if (isMain) {
  Main()
}
