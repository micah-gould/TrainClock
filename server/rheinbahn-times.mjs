import fetch from "node-fetch"

export async function getDepartures(id) {
  try {
    id = id ?? 653379  
    const filter = a => a.stop.location.latitude === 51.312882
    const getTime = departure => {
      const when = (departure.when || departure.plannedWhen)?.slice(11, -6)?.split(":") ?? new Date()
      const now = new Date()
      return (parseInt(when[0] - now.getHours())*60 + parseInt(when[1] - now.getMinutes()) - 1)
    }
    const URL = `https://v6.db.transport.rest/stops/${id}/departures?duration=30`
    const response = await fetch(URL)
    const departures = (await response?.json() ?? [-1, -1])?.departures?.filter(filter) ?? [-1, -1]

    const nextTime = getTime(departures[0] ?? -1)
    return nextTime > 0 ? nextTime : getTime(departures[1] ?? -1)
  } catch (e) {
    return -1
  }
}

console.log (await getDepartures(process.argv.slice(2)[0]) ?? 653379)
