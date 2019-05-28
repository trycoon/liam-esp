import * as api from '../api.js';

const MAX_SCHEDULE_ENTRIES = 10;
let scheduleListEl = document.getElementById('scheduleList'),
    scheduleEntries = 0;

/**
 * Generate HTML fragment for a schedule entry.
 * @param {Number} position position in schedule list
 * @param {Array} activeWeekdays array of boolean, where each position represent a weekday and wether that day is selected.
 * @param {String} startTime HH:MM of when mower should start mowing
 * @param {String} stopTime HH:MM of when mower should stop mowing
 * @returns {String} HTML fragment
 */
function getScheduleEntryFragment(position, activeWeekdays, startTime, stopTime) {

    let dayList = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'].map((day, index) => {
        return `<span class="day ${activeWeekdays[index] ? 'selected' : ''}">${day}</span>`;
    }).join('');

    return `
      <li class="center">
        <div>
          ${dayList}
        </div>
        <div>
          <span>${startTime} - ${stopTime}</span>
          <div class="js-removeSchedule removeSchedule" data-pos="${position}">
            <i title="remove schedule" class="far fa-trash-alt"></i>
          </div>
        </div>
      </li>
    `
}

function addScheduleEntry() {

  const timeRegExp = /(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23):(0|1|2|3|4|5)\d/g;
  let startTime = `${document.getElementById('js-startHour').value}:${document.getElementById('js-startMin').value}`,
      stopTime = `${ document.getElementById('js-stopHour').value}:${document.getElementById('js-stopMin').value}`,
      weekDayButtons = [ ...document.querySelectorAll('#js-addScheduleButtons input') ],
      activeWeekdays = weekDayButtons.map(day => {
        return day.checked;
      });

  // check that atleast one day of week is selected, that start- and stoptime is valid, and that there is no more than 10 schedules.
  if (!activeWeekdays.every(selected => {return selected === false}) && startTime.match(timeRegExp) && stopTime.match(timeRegExp) && scheduleEntries < MAX_SCHEDULE_ENTRIES) {
    $.when(api.addScheduleEntry(activeWeekdays, startTime, stopTime))
    .done(() => {
      renderScheduleList();
    });
  }
}

function removeScheduleEntry(event) {

  $.when(api.removeScheduleEntry(event.currentTarget.dataset.pos))
  .done(() => {
    renderScheduleList();
  });

}

function renderScheduleList() {

  $.when(api.getScheduleList())
  .done(schedules => {
    // remove old list first
    [ ...scheduleListEl.childNodes ].forEach(el => el.remove());

    scheduleEntries = schedules.length;

    document.querySelector('.js-activeSchedules').textContent = `(${scheduleEntries}/${MAX_SCHEDULE_ENTRIES})`;
    
    schedules.forEach((entry, index) => {

      let listItem = document.createRange().createContextualFragment(getScheduleEntryFragment(index, entry.activeWeekdays, entry.startTime, entry.stopTime));
      listItem.querySelector('.js-removeSchedule').addEventListener('click', removeScheduleEntry);      
      
      scheduleListEl.appendChild(listItem);
    
    });

    if (schedules.length === 0) {
      scheduleListEl.appendChild(document.createTextNode("No active schedule! Add one above."));
    } 
  });   

}

export function selected() {

  renderScheduleList();

}

export function unselected() {
   
}

export function init() {

  document.getElementById('js-addSchedule').addEventListener('click', addScheduleEntry);

}
