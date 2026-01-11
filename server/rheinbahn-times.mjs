import fetch from 'node-fetch'

const DEFAULT_STOP_ID = 653379
const DEFAULT_DIRECTION_ID = 653647
const DEFAULT_LATITUDE = 51.312882

export async function getDepartures (stopID = DEFAULT_STOP_ID, directionID = DEFAULT_DIRECTION_ID, count = 0) {
  try {
    const filter = a => a.stop.location.latitude === DEFAULT_LATITUDE
    const getTime = departure => {
      const when = (departure.when || departure.plannedWhen)?.slice(11, -6)?.split(':') ?? new Date()
      const now = new Date()
      return (parseInt(when[0] - now.getHours()) * 60 + parseInt(when[1] - now.getMinutes()) - 1)
    }
    const URL = `https://v6.db.transport.rest/stops/${stopID}/departures?results=2&direction=${directionID}`
    console.log(URL)
    const response = await fetch(URL)
    const departures = (await response?.json() ?? [-1, -1])?.departures?.filter(filter) ?? [-1, -1]

    const nextTime = getTime(departures[0] ?? -1)
    return nextTime > 0 ? nextTime : getTime(departures[1] ?? -1)
  } catch (e) {
    console.log(e)
    if (count > 5) return -1
    setTimeout(() => getDepartures(stopID, directionID, count + 1), 1000)
  }
}

async function Main() {
  console.log(await getDepartures(process.argv.slice(2)[0] ?? DEFAULT_STOP_ID, process.argv.slice(2)[1] ?? DEFAULT_DIRECTION_ID))
}

if (require.main == module) {
  Main()
}
